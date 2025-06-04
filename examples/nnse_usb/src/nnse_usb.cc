// #include "tflite.h"
#include <stdint.h>
#include "seCntrlClass.h"
#include "am_util_stdio.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_audio.h"
#include "ambiq_nnsp_const.h"
#include "ns_timer.h"
#include "ns_energy_monitor.h"
#include "nn_speech.h"
#include "ns_perf_profile.h"
#include "ns_usb.h"

#include "ae_api.h"
#include "FreeRTOS.h"
#include "task.h"
#include "arm_math.h"

#include "usb_packet_generated.h"
// #include "audio_webble.h"

alignas(16) unsigned char static encodedDataBuffer[80]; // Opus encoder output length is hardcoded
                                                        // to 80 bytes
bool enableSE = false; // Flip between SE and Raw audio when button 0 is pressed
uint32_t seLatency = 0;
uint32_t opusLatency = 0;

#if (configAPPLICATION_ALLOCATED_HEAP == 1)
    #define NNSE_HEAP_SIZE (40 * 1024)
size_t ucHeapSize = NNSE_HEAP_SIZE;
uint8_t ucHeap[NNSE_HEAP_SIZE] __attribute__((aligned(4)));
#endif

#define NUM_CHANNELS 1

// #define USE_AUDADC // Uncomment this to use the AUDADC instead of the PDM

/// Button Peripheral Config
int volatile g_intButtonPressed = 0;
ns_button_config_t button_config_nnsp = {
    .api = &ns_button_V1_0_0,
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &g_intButtonPressed,
    .button_1_flag = NULL};

/// Audio Config
bool volatile static g_audioRecording = false;
bool volatile static g_audioReady = false;
alignas(16) int16_t static g_in16AudioDataBuffer[LEN_STFT_HOP << 1];
alignas(16) uint32_t static audadcSampleBuffer[(LEN_STFT_HOP << 1) + 3];
#ifdef USE_AUDADC
alignas(16) am_hal_audadc_sample_t static workingBuffer[SAMPLES_IN_FRAME * NUM_CHANNELS]; // working buffer
                                                                              // used by AUDADC
#endif
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
am_hal_offset_cal_coeffs_array_t sOffsetCalib;
#endif

// Use this to generate a sinwave for debugging instead
// of using the microphone
alignas(16) int16_t static sinWave[320];

// WebUSB Configuration and Datatypes
#define MY_RX_BUFSIZE 4096
#define MY_TX_BUFSIZE 4096

static uint8_t my_rx_ff_buf[MY_RX_BUFSIZE] __attribute__((aligned(16)));
static uint8_t my_tx_ff_buf[MY_TX_BUFSIZE] __attribute__((aligned(16)));
// WebUSB URL
static ns_tusb_desc_webusb_url_t webusb_url;
static ns_usb_config_t webUsbConfig = {
    .api = &ns_usb_V1_0_0,
    .deviceType = NS_USB_VENDOR_DEVICE,
    .rx_buffer = NULL,
    .rx_bufferLength = 0,
    .tx_buffer = NULL,
    .tx_bufferLength = 0,
    .rx_cb = NULL,
    .tx_cb = NULL,
    .service_cb = NULL,
    .desc_url = &webusb_url // Filled in at runtime
};

void audio_frame_callback(ns_audio_config_t *config, uint16_t bytesCollected) {
    if (g_audioRecording) {
        ns_audio_getPCM_v2(config, g_in16AudioDataBuffer);
        g_audioReady = true;
    }
}

ns_audio_config_t audio_config = {
    .api = &ns_audio_V2_0_0,
    .eAudioApiMode = NS_AUDIO_API_CALLBACK,
    .callback = audio_frame_callback,
    .audioBuffer = (void *)&g_in16AudioDataBuffer,
#ifdef USE_AUDADC
    .eAudioSource = NS_AUDIO_SOURCE_AUDADC,
#else
    .eAudioSource = NS_AUDIO_SOURCE_PDM,
#endif
    .sampleBuffer = audadcSampleBuffer,
#ifdef USE_AUDADC
    .workingBuffer = workingBuffer,
#else
    .workingBuffer = NULL,
#endif
    .numChannels = NUM_CHANNELS,
    .numSamples = LEN_STFT_HOP,
    .sampleRate = SAMPLING_RATE,
    .audioSystemHandle = NULL, // filled in by init
    .bufferHandle = NULL,      // only for ringbuffer mode
#if !defined(NS_AMBIQSUITE_VERSION_R4_1_0) && defined(NS_AUDADC_PRESENT)
    .sOffsetCalib = &sOffsetCalib,
#endif
};

// Custom power mode for USB+Audio
const ns_power_config_t ns_power_usb = {
    .api = &ns_power_V1_0_0,
    .eAIPowerMode = NS_MAXIMUM_PERF,
    .bNeedAudAdc = false,
    .bNeedSharedSRAM = true,
    .bNeedCrypto = false,
    .bNeedBluetooth = false,
    .bNeedUSB = true,
    .bNeedIOM = false,
    .bNeedAlternativeUART = false,
    .b128kTCM = false,
    .bEnableTempCo = false,
    .bNeedITM = true,
    .bNeedXtal = true};

// Speech Enhancement Control Structure
seCntrlClass cntrl_inst;

// Transmit Buffer
// Opus frames are 20ms, so we need 320 samples at 16kHz
// AudioTask writes clean audio to xmitBuffer
// EndodeAndXferTask watches xmitBuffer, and encodes it when there are 320 samples available (and
// sends that)
#define XBUFSIZE 640
alignas(16) int16_t static xmitBuffer[XBUFSIZE];
uint16_t static xmitWritePtr = 0;
uint16_t static xmitReadPtr = 0;
uint16_t static xmitAvailable = 0;

// FreeRTOS Tasks
TaskHandle_t audio_task_handle;  // Collects audio and de-noises it
TaskHandle_t radio_task_handle;  // Services WSF for BLE
TaskHandle_t encode_task_handle; // Encodes audio and sends it over BLE
TaskHandle_t my_xSetupTask;      // Starts the other tasks and suspends itself

// Timer and structs for performance profiling
// ns_perf_counters_t start, end, delta;
// int8_t do_it_once = 1;
ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};

usb_data_t data;

// Audio Task
uint32_t seStart, seEnd;
uint32_t opusStart, opusEnd;
uint32_t seLatencyCapturePeriod = 10;  // measure every 100 frames (1s)
uint32_t opusLatencyCapturePeriod = 5; // measure every 100 frames (1s)
uint32_t currentSESample = 0;
uint32_t currentOpusSample = 0;

// Structure for tracking acknowledgements
struct ack_tracker_t {
    uint16_t sequence;
    bool received;
};

static ack_tracker_t current_ack = {0, false};

// Function to wait for acknowledgement
bool wait_for_ack(uint16_t sequence, uint32_t timeout_ms) {
    uint32_t start = ns_us_ticker_read(&basic_tickTimer);
    while ((ns_us_ticker_read(&basic_tickTimer) - start) < (timeout_ms * 1000)) {
        if (current_ack.received && current_ack.sequence == sequence) {
            current_ack.received = false;
            return true;
        }
        ns_delay_us(100);
    }
    return false;
}

// Callback for received messages
void msgReceived(const uint8_t *buffer, uint32_t length, void *args) {
    if (length < sizeof(usb_message_header_t)) return;
    
    // Extract header
    usb_message_header_t* header = (usb_message_header_t*)buffer;
    const uint8_t* payload = buffer + sizeof(usb_message_header_t);
    size_t payload_len = length - sizeof(usb_message_header_t);
    
    // Verify CRC
    uint32_t computed_crc = calcCrc32(0, payload_len, (uint8_t*)payload);
    if (computed_crc != header->crc32) {
        ns_lp_printf("CRC mismatch\n");
        return;
    }
    
    // Parse FlatBuffer
    auto fb_packet = web::usb::GetUsbPacket(payload);
    
    // Check if this is an acknowledgement
    if (fb_packet->type() == web::usb::UsbDataType_NONE && 
        fb_packet->data_length() == 1 && 
        fb_packet->data()->Get(0) == 1) {
        current_ack.sequence = fb_packet->sequence();
        current_ack.received = true;
    }
}


// High Level Transport Layer

// Message header structure
typedef struct {
    uint32_t crc32;
    uint8_t command;
    uint8_t chunk_id;
    uint8_t total_chunks;
} __attribute__((packed)) usb_message_header_t;

// USB Transport Layer Constants
#define USB_MAX_PACKET_SIZE     512  // Max WebUSB packet size
#define USB_HEADER_SIZE         sizeof(usb_message_header_t)  // 7 bytes
#define ALIGNMENT_MARGIN        8    // Safety margin for alignment

#define FLATBUFFER_VTABLE_SIZE  4    // vtable overhead
#define FLATBUFFER_FIELDS_SIZE  12   // type(1) + platform(1) + sequence(2) + version(1) + vector_len(4) + padding(3)

// Calculate maximum data chunk size
#define MAX_PAYLOAD      (USB_MAX_PACKET_SIZE - \
                                USB_HEADER_SIZE - \
                                FLATBUFFER_VTABLE_SIZE - \
                                FLATBUFFER_FIELDS_SIZE - \
                                ALIGNMENT_MARGIN)  // ~481 bytes

// // Function to calculate maximum chunk size based on current FlatBuffer schema
// size_t calculate_max_chunk_size() {
//     flatbuffers::FlatBufferBuilder builder;
    
//     // Create a minimal packet to measure overhead
//     auto dummy_vector = builder.CreateVector((uint8_t*)nullptr, 0);
//     auto packet = web::usb::CreateUsbPacket(
//         builder,
//         web::usb::UsbDataType_NONE,
//         (web::usb::Platform)data.platform,
//         0,
//         1,
//         dummy_vector
//     );
//     builder.Finish(packet, "USB1");
    
//     // Get FlatBuffer overhead (everything except the actual data)
//     size_t flatbuffer_overhead = builder.GetSize();
    
//     // Calculate maximum data size that can fit in a packet
//     return USB_MAX_PACKET_SIZE - USB_HEADER_SIZE - flatbuffer_overhead - ALIGNMENT_MARGIN;
// }

// Helper function to send chunked data with retries
void send_chunked_data(web::usb::UsbDataType type, const uint8_t* data, size_t data_len) {
    static size_t max_chunk_size = MAX_PAYLOAD;
    
    // // Calculate max chunk size once
    // if (max_chunk_size == 0) {
    //     max_chunk_size = calculate_max_chunk_size();
    //     ns_lp_printf("Calculated max chunk size: %d bytes\n", max_chunk_size);
    // }
    
    // Basically math.ceil(data_len / max_chunk_size)
    const size_t num_chunks = (data_len + max_chunk_size - 1) / max_chunk_size;
    
    for (size_t chunk = 0; chunk < num_chunks; chunk++) {
        size_t chunk_size = (chunk == num_chunks - 1) ? 
            (data_len - chunk * max_chunk_size) : max_chunk_size;
        
        // Create FlatBuffer for this chunk
        flatbuffers::FlatBufferBuilder builder;
        
        // Create data vector for this chunk
        auto data_vector = builder.CreateVector(
            data + (chunk * max_chunk_size), 
            chunk_size
        );
        
        // Create USB packet - using chunk as sequence number
        auto packet = web::usb::CreateUsbPacket(
            builder,
            type,
            (web::usb::Platform)data.platform,
            chunk,  // Use chunk as sequence number
            1, // version
            data_vector
        );
        builder.Finish(packet, "USB1");
        
        // Get the serialized buffer
        uint8_t* buf = builder.GetBufferPointer();
        size_t len = builder.GetSize();
        
        // Create header - chunk_id and sequence are now the same
        usb_message_header_t header;
        header.crc32 = calcCrc32(0, len, buf);
        header.command = 0; // SEND command
        header.chunk_id = chunk;
        header.total_chunks = num_chunks;
        
        // Combine header and data
        uint8_t tx_buf[sizeof(header) + len];
        memcpy(tx_buf, &header, sizeof(header));
        memcpy(tx_buf + sizeof(header), buf, len);
        
        // Send with retries
        bool ack_received = false;
        int retries = 0;
        const int MAX_RETRIES = 3;
        
        while (!ack_received && retries < MAX_RETRIES) {
            webusb_send_data(tx_buf, sizeof(tx_buf));
            
            // Wait for acknowledgement - now waiting for chunk number
            if (wait_for_ack(chunk, 100)) { // Using chunk instead of sequence_number
                ack_received = true;
            } else {
                retries++;
                ns_delay_us(1000 * (1 << retries)); // Exponential backoff
            }
        }
        
        if (!ack_received) {
            ns_lp_printf("Failed to send chunk %d after %d retries\n", chunk, MAX_RETRIES);
            return;
        }
    }
}

// USB Senders
void set_se_mode(bool enable) {
    ns_lp_printf("SE Mode %d\n", enable);
    data.type = SET_SE_MODE;
    data.length = 1;
    data.data[0] = enable ? 1 : 0;
    webusb_send_data((uint8_t *)&data, 5);
}

void send_se_latency(uint32_t latency) {
    data.type = SE_LATENCY;
    data.length = sizeof(latency);
    // ns_lp_printf("SE Latency %d\n", latency);
    memcpy(data.data, &latency, sizeof(latency));
    webusb_send_data((uint8_t *)&data, 7);
}
void send_opus_latency(uint32_t latency) {
    data.type = OPUS_LATENCY;
    data.length = sizeof(latency);
    // ns_lp_printf("Opus Latency %d\n", latency);
    memcpy(data.data, &latency, sizeof(latency));
    webusb_send_data((uint8_t *)&data, 7);
}
void send_audio_data(unsigned char  *buffer) {
    data.type = AUDIO_DATA;
    data.length = 80;
    memcpy(data.data, buffer, 80);
    // ns_lp_printf("Sending %d bytes\n", sizeof(usb_data_t));
    webusb_send_data((uint8_t *)&data, sizeof(usb_data_t));
    ns_delay_us(600);
}

void audioTask(void *pvParameters) {
    while (1) {
        if (g_intButtonPressed) {
            // Flip modes between SE and Raw audio
            enableSE = !enableSE;
            set_se_mode(enableSE);
            // ns_ble_send_value(&bleSEEnabled, NULL);
            g_intButtonPressed = 0;
        }
        // ns_lp_printf("AudioTask: xmitAvailable %d\n", xmitAvailable);
        if (g_audioReady) { // Every time audio frame is available (160 samples, 10ms)

            NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed. ");
            if (currentSESample == seLatencyCapturePeriod) {
                seStart = ns_us_ticker_read(&basic_tickTimer);
            }

            // SE Model is stateful, so you have to call it with every frame
            // even if you don't want SE's output (otherwise it'll sound wierd for a bit at
            // the beginning)
            seCntrlClass_exec(&cntrl_inst, g_in16AudioDataBuffer, xmitBuffer + xmitWritePtr);

            if (currentSESample == seLatencyCapturePeriod) {
                seEnd = ns_us_ticker_read(&basic_tickTimer);
                seLatency = seEnd - seStart;
                send_se_latency(seLatency);
                // ns_ble_send_value(&bleSELatency, NULL);
                currentSESample = 0;
            } else {
                currentSESample++;
            }

            if (!enableSE) {
                // Overwrite the SE output with the original audio
                memcpy(
                    xmitBuffer + xmitWritePtr, g_in16AudioDataBuffer,
                    LEN_STFT_HOP * sizeof(int16_t));
            }

            NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

            // Add it to our simple ring buffer
            xmitAvailable += 160;
            xmitWritePtr = (xmitWritePtr + 160) % XBUFSIZE;
            g_audioReady = false;
        }
        vTaskDelay(pdMS_TO_TICKS(1)); // this could be probably be slower, but we want to keep the
                                      // audio latency low
    }
}

// Encode and Xfer Task
void encodeAndXferTask(void *pvParameters) {
    uint32_t ret;

    while (1) {
        // ns_lp_printf("XmitAvailable %d\n", xmitAvailable);
        if (xmitAvailable >= 320) {
            NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed. ");
            if (currentOpusSample == opusLatencyCapturePeriod) {
                opusStart = ns_us_ticker_read(&basic_tickTimer);
            }

            ret = audio_enc_encode_frame(xmitBuffer + xmitReadPtr, 320, encodedDataBuffer);
            NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

            if (currentOpusSample == opusLatencyCapturePeriod) {
                opusEnd = ns_us_ticker_read(&basic_tickTimer);
                opusLatency = opusEnd - opusStart;
                send_opus_latency(opusLatency);
                // ns_ble_send_value(&bleOpusLatency, NULL);
                currentOpusSample = 0;
            } else {
                currentOpusSample++;
            }

            if (ret >= 0) {
                send_audio_data(encodedDataBuffer);
                // ns_ble_send_value(&webbleOpusAudio, NULL); // Send the encoded audio over BLE
            } else {
                ns_lp_printf("Error encoding %d\n", ret);
            }
            xmitAvailable -= 320;
            xmitReadPtr = (xmitReadPtr + 320) % XBUFSIZE;
        }
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

void setup_task(void *pvParameters) {
    ns_lp_printf("Setting up USB FreeRTOS Tasks\n");
    // xTaskCreate(RadioTask, "RadioTask", 512, 0, 3, &radio_task_handle);
    xTaskCreate(audioTask, "AudioTask", 2048, 0, 3, &audio_task_handle);
    xTaskCreate(encodeAndXferTask, "encodeAndXferTask", 4096, 0, 3, &encode_task_handle);
    vTaskSuspend(NULL);
    while (1)
        ;
}

int main(void) {
    usb_handle_t usb_handle = NULL;

    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\b");

    ns_power_config(&ns_power_usb);

    // Only turn HP while doing codec and AI
    NS_TRY(ns_set_performance_mode(NS_MINIMUM_PERF), "Set CPU Perf mode failed. ");

    ns_itm_printf_enable();
    ns_interrupt_master_enable();

    ns_audio_init(&audio_config);
    ns_peripheral_button_init(&button_config_nnsp);
    ns_init_perf_profiler();
    ns_start_perf_profiler();

    // Initialize the Opus encoder
    audio_enc_init(0);

    // Initialize the URL descriptor
    strcpy(webusb_url.url, "ambiqai.github.io/web-ble-dashboards/nnse-usb/");
    webusb_url.bDescriptorType = 3;
    webusb_url.bScheme = 1;
    webusb_url.bLength = 3 + sizeof(webusb_url.url) - 1;

    // WebUSB Setup
    webusb_register_raw_cb(msgReceived, NULL);
    webUsbConfig.rx_buffer = my_rx_ff_buf;
    webUsbConfig.rx_bufferLength = MY_RX_BUFSIZE;
    webUsbConfig.tx_buffer = my_tx_ff_buf;
    webUsbConfig.tx_bufferLength = MY_TX_BUFSIZE;

    NS_TRY(ns_usb_init(&webUsbConfig, &usb_handle), "USB Init Failed\n");
    ns_lp_printf("USB Init Success\n");

    // Initialize the model, get handle if successful
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");
    // while(1);
    // Generate a 400hz sin wave (for debugging)
    for (int i = 0; i < 320; i++) {
        sinWave[i] = (int16_t)(sin(2 * 3.14159 * 400 * i / SAMPLING_RATE) * 32767);
    }

    #if defined(AM_PART_APOLLO5B)
    data.platform = 2;
    #elif defined(AM_PART_APOLLO4P)
    data.platform = 1;
    #else
    data.platform = 0;
    #endif

    // initialize neural nets controller
    seCntrlClass_init(&cntrl_inst);
    // BLE is FreeRTOS-driven, everything happens in the tasks set up by setup_task()
    // Audio xmit will start immediately, no waiting for button presses
    g_audioRecording = true;
    xTaskCreate(setup_task, "Setup", 512, 0, 1, &my_xSetupTask);

    vTaskStartScheduler();
    while (1) {
    };
}
