/**
 * @file ic_bench_example.cc
 * @author Carlos Morales
 * @brief Minimal application instantiating a TFLM model, feeding it
 * a test input tensor, and checking the result
 * @version 0.1
 * @date 2023-02-28
 *
 * @copyright Copyright (c) 2023
 *
 */

// alignas(16) unsigned char ic_bench_model[98640];
unsigned char *ic_bench_model;


#include "ns_core.h"
#include "ns_ambiqsuite_harness.h"
#include "ns_energy_monitor.h"
#include "ns_peripherals_power.h"
#include "ns_peripherals_button.h"
#include "ns_rpc_generic_data.h"
#include "am_devices_mspi_psram_aps25616n.h"

volatile uint32_t model_chunk_offset = 0;

#define MY_RX_BUFSIZE 4096
#define MY_TX_BUFSIZE 4096
uint8_t my_rx_ff_buf[MY_RX_BUFSIZE];
uint8_t my_tx_ff_buf[MY_TX_BUFSIZE];
#if (configAPPLICATION_ALLOCATED_HEAP == 1)
size_t ucHeapSize = (NS_RPC_MALLOC_SIZE_IN_K + 4) * 1024;
uint8_t ucHeap[(NS_RPC_MALLOC_SIZE_IN_K + 4) * 1024] __attribute__((aligned(4)));
#endif

// Model stuff
#include "ic_bench_api.h"
#include "ic_bench_example_tensors.h"
#include "examples.h"
#include "ns_model.h"

const char* kCategoryLabels[11] = {
	"airplane",
	"automobile",
	"bird",
	"cat",
	"deer",
	"dog",
	"frog",
	"horse",
	"ship",
	"truck",
    "unknown"
};

// TFLM Config
static ns_model_state_t model;
volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

ns_timer_config_t basic_tickTimer = {
    .api = &ns_timer_V1_0_0,
    .timer = NS_TIMER_COUNTER,
    .enableInterrupt = false,
};



#define DDR_PSRAM_TARGET_ADDRESS 0
#define MSPI_BUFFER_SIZE        (4*1024)  // 4K example buffer size.
#define DEFAULT_TIMEOUT         10000
#define MSPI_PSRAM_OCTAL_CONFIG MSPI_PSRAM_OctalCE0MSPIConfig

AM_SHARED_RW uint32_t        DMATCBBuffer[2560];
AM_SHARED_RW uint8_t         TestBuffer[2048];
AM_SHARED_RW uint8_t         DummyBuffer[1024];
AM_SHARED_RW uint8_t         g_TXBuffer[MSPI_BUFFER_SIZE];
AM_SHARED_RW uint8_t         g_RXBuffer[MSPI_BUFFER_SIZE];
void            *g_pDevHandle;
void            *g_pHandle;

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE0MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE0,
    .eClockFreq               = AM_HAL_MSPI_CLK_125MHZ,
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

am_devices_mspi_psram_config_t MSPI_PSRAM_OctalCE1MSPIConfig =
{
    .eDeviceConfig            = AM_HAL_MSPI_FLASH_OCTAL_DDR_CE1,
    .eClockFreq               = AM_HAL_MSPI_CLK_125MHZ,
    .pNBTxnBuf                = DMATCBBuffer,
    .ui32NBTxnBufLength       = sizeof(DMATCBBuffer) / sizeof(uint32_t),
    .ui32ScramblingStartAddr  = 0,
    .ui32ScramblingEndAddr    = 0,
};

am_devices_mspi_psram_ddr_timing_config_t MSPIDdrTimingConfig;

#define MSPI_TEST_MODULE              3

#if (MSPI_TEST_MODULE == 0)
#define MSPI_XIP_BASE_ADDRESS MSPI0_APERTURE_START_ADDR
#elif (MSPI_TEST_MODULE == 1)
#define MSPI_XIP_BASE_ADDRESS MSPI1_APERTURE_START_ADDR
#elif (MSPI_TEST_MODULE == 2)
#define MSPI_XIP_BASE_ADDRESS MSPI2_APERTURE_START_ADDR
#elif (MSPI_TEST_MODULE == 3)
#define MSPI_XIP_BASE_ADDRESS MSPI3_APERTURE_START_ADDR
#endif // #if (MSPI_TEST_MODULE == 0)

//! MSPI interrupts.
static const IRQn_Type mspi_interrupts[] =
{
    MSPI0_IRQn,
    MSPI1_IRQn,
    MSPI2_IRQn,
    MSPI3_IRQn,
};

#define psram_mspi_isr                                                          \
    am_mspi_isr1(MSPI_TEST_MODULE)
#define am_mspi_isr1(n)                                                        \
    am_mspi_isr(n)
#define am_mspi_isr(n)                                                         \
    am_mspi ## n ## _isr

// MSPI ISRs.
void psram_mspi_isr(void)
{
    uint32_t      ui32Status;
    am_hal_mspi_interrupt_status_get(g_pHandle, &ui32Status, false);
    am_hal_mspi_interrupt_clear(g_pHandle, ui32Status);
    am_hal_mspi_interrupt_service(g_pHandle, ui32Status);
}

void psram_init(void) {
    uint32_t      ui32Status;

    am_util_stdio_printf("Starting MSPI DDR Timing Scan: \n");
    if ( AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS == am_devices_mspi_psram_aps25616n_ddr_init_timing_check(MSPI_TEST_MODULE, &MSPI_PSRAM_OCTAL_CONFIG, &MSPIDdrTimingConfig) )
    {
        am_util_stdio_printf("==== Scan Result: RXDQSDELAY0 = %d \n", MSPIDdrTimingConfig.ui32Rxdqsdelay);
    }
    else
    {
        am_util_stdio_printf("==== Scan Result: Failed, no valid setting.  \n");
        while(1);
    }

    ui32Status = am_devices_mspi_psram_aps25616n_ddr_init(MSPI_TEST_MODULE, &MSPI_PSRAM_OCTAL_CONFIG, &g_pDevHandle, &g_pHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to configure the MSPI and PSRAM Device correctly!\n");
        am_util_stdio_printf("Must ensure Device is Connected and Properly Initialized!\n");
        while(1);
    }

    NVIC_SetPriority(mspi_interrupts[MSPI_TEST_MODULE], AM_IRQ_PRIORITY_DEFAULT);
    NVIC_EnableIRQ(mspi_interrupts[MSPI_TEST_MODULE]);
    am_hal_interrupt_master_enable();

    am_devices_mspi_psram_aps25616n_apply_ddr_timing(g_pDevHandle, &MSPIDdrTimingConfig);

    //
    // Enable XIP mode.
    //
    ui32Status = am_devices_mspi_psram_aps25616n_ddr_enable_xip(g_pDevHandle);
    if (AM_DEVICES_MSPI_PSRAM_STATUS_SUCCESS != ui32Status)
    {
        am_util_stdio_printf("Failed to enable XIP mode in the MSPI!\n");
    }
}

status incomingModelChunk(const dataBlock *in) {
    ns_lp_printf(
        "[INFO] PC Sent Model Chunk of %d bytes, copied to %d\n", in->buffer.dataLength,
        model_chunk_offset);
    // Get latest chunk, copy into next spot in model array
    memcpy(&ic_bench_model[model_chunk_offset], in->buffer.data, in->buffer.dataLength);
    
    // Check that it matches
    if (0 != memcmp(&ic_bench_model[model_chunk_offset], in->buffer.data, in->buffer.dataLength)) {
        ns_lp_printf("Mismatch! Model chunk offset: %d, addr 0x%x\n", model_chunk_offset, &ic_bench_model[model_chunk_offset]);
    }

    model_chunk_offset += in->buffer.dataLength;
    ns_lp_printf("Model chunk offset: %d\n", model_chunk_offset);
    return ns_rpc_data_success;
}

int main(void) {
    ns_core_config_t ns_core_cfg = {.api = &ns_core_V1_0_0};
    usb_handle_t usb_handle = NULL;
    volatile int g_intButtonPressed = 0;
    // ns_button_config_t button_config = {
    //     .api = &ns_button_V1_0_0,
    //     .button_0_enable = true,
    //     .button_1_enable = false,
    //     .button_0_flag = &g_intButtonPressed,
    //     .button_1_flag = NULL};
    char *source;
    int8_t max, label, j;
    j = 0;
    int invokes = 0;
    uint32_t oldTime = ns_us_ticker_read(&basic_tickTimer);
    uint32_t newTime = oldTime;
    float ips = 0;

    // Initialize the platform
    NS_TRY(ns_core_init(&ns_core_cfg), "Core init failed.\n");
    ns_interrupt_master_enable();
    am_bsp_itm_printf_enable();

    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed.\n");
    // NS_TRY(ns_set_performance_mode(NS_MAXIMUM_PERF), "Set CPU Perf mode failed.");
    ns_itm_printf_enable();
    NS_TRY(ns_timer_init(&basic_tickTimer), "Timer init failed.\n");

    ns_rpc_config_t rpcConfig = {
        .api = &ns_rpc_gdo_V1_0_0,
        .mode = NS_RPC_GENERICDATA_SERVER, // Puts EVB in RPC server mode
        .rx_buf = my_rx_ff_buf,
        .rx_bufLength = 4096,
        .tx_buf = my_tx_ff_buf,
        .tx_bufLength = 4096,
        .sendBlockToEVB_cb = incomingModelChunk,
        .fetchBlockFromEVB_cb = NULL,
        .computeOnEVB_cb = NULL};
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n"); // init RPC and USB

    // Initialize the PSRAM
    psram_init();
    ic_bench_model = (unsigned char *)MSPI_XIP_BASE_ADDRESS;
    ns_lp_printf("Model Address: 0x%x\n", ic_bench_model);

    // Wait for button before loading model
    ns_lp_printf("Run Model Loader\n");

    // Wait for model to finish loading
    while (model_chunk_offset < 98640) {
        ns_rpc_genericDataOperations_pollServer(&rpcConfig);
        ns_delay_us(1000);
    }
    ns_lp_printf("Model loaded (%d bytes)\n", model_chunk_offset);
    ns_lp_printf("USB Init Success\n");
    for (int i = 0; i < 100; i++) {
        ns_lp_printf("Model loaded (%d bytes)\n", model_chunk_offset);
        example_status = ic_bench_STATUS_SUCCESS;
    }

    // Initialize the model, get handle if successful
    int status = ic_bench_minimal_init(&model); // model init with minimal defaults
    if (status == ic_bench_STATUS_FAILURE) {
        ns_lp_printf("Model init failed... status %d\n", status);
        while (1)
            example_status = ic_bench_STATUS_INIT_FAILED; // hang
    }

    // Loop through 100 images 
    while (1) {
        source = (char *)&(images[j][0]);
        memcpy(model.model_input[0]->data.int8, source, model.model_input[0]->bytes);

        example_status = model.interpreter->Invoke();
        invokes++;
        newTime = ns_us_ticker_read(&basic_tickTimer);
        if (newTime - oldTime > 1000000) {
            ips = (float)invokes / ((float)(newTime - oldTime) / 1000000.0f);
            invokes = 0;
            oldTime = newTime;
            ns_lp_printf("Image Classification FPS: %0.2f\n", ips);
        }   
        
        // Parse the output tensor to find max value
        max = -127;
        label = 0;
        for(uint8_t i =0; i< model.model_output[0]->bytes; i++)
        {
            if (model.model_output[0]->data.int8[i] > max)
            {
                max = model.model_output[0]->data.int8[i];
                label = i;
            }
        }

        // Send the result to WebUSB client 
        uint8_t confidence = (uint8_t)((max + 127) * 100 / 255);
        // convert ips to uint8_t
        uint8_t fps = ips;
        // sendMessage(1, label, confidence, fps, 0, 0);
        ns_lp_printf("%d - Label is %d (%s). FPS = %0.2f\n", j, label, kCategoryLabels[label], ips);
        ns_lp_printf("Text is %s\n", kCategoryLabels[label]);

        // Wrap at 100
        j = (j + 1)%100; 
    }
    while (1) {
        // Success!
        example_status = ic_bench_STATUS_SUCCESS;
    }
}
