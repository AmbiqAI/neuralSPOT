#include "ns_ambiqsuite_harness.h"
#include "ns_peripherals_button.h"
#include "ns_peripherals_power.h"
#include "ns_rpc_generic_data.h"

#include "ns_core.h"

#include "tts.h"
#include "fuse2mel_init.h"
#include "phone2fuse_init.h"
#include "tflm_ns_model.h"

#define NUM_TOKENS 31
#define TIMESTEPS 300
#define DIM_FUSE 128
#define BYTES_INPUT_SIZE_F2M 76800

volatile int g_intButtonPressed = 0;
ns_button_config_t button_config = {
    .api = &ns_button_V1_0_0,
    .button_0_enable = true,
    .button_1_enable = false,
    .button_0_flag = &g_intButtonPressed,
    .button_1_flag = NULL};

// Block sent to PC

static int32_t audioDataBuffer[256];
static binary_t binaryBlock = {
        .data = (uint8_t *)audioDataBuffer, // point this to audio buffer
        .dataLength = sizeof(audioDataBuffer)};

static char msg_store[50] = "Audio32bPCM_Mono_to_WAV";
static char msg_compute[30] = "CalculateMFCC_Please";

static dataBlock outBlock = {
    .length = sizeof(audioDataBuffer),
    .dType = uint8_e,
    .description = msg_store,
    .cmd = write_cmd,
    .buffer = binaryBlock};

// Block sent to PC for computation
static dataBlock computeBlock = {
    .length =  sizeof(audioDataBuffer),
    .dType = uint8_e,
    .description = msg_compute,
    .cmd = extract_cmd,
    .buffer = binaryBlock};

#define MY_USB_RX_BUFSIZE 2048  
#define MY_USB_TX_BUFSIZE 2048
static uint8_t my_cdc_rx_ff_buf[MY_USB_RX_BUFSIZE];
static uint8_t my_cdc_tx_ff_buf[MY_USB_TX_BUFSIZE];
ns_rpc_config_t rpcConfig = {
    .api = &ns_rpc_gdo_V1_1_0,
    .mode = NS_RPC_GENERICDATA_CLIENT,
    .rx_buf = my_cdc_rx_ff_buf,
    .rx_bufLength = MY_USB_RX_BUFSIZE,
    .tx_buf = my_cdc_tx_ff_buf,
    .tx_bufLength = MY_USB_TX_BUFSIZE,
#ifndef NS_USB_PRESENT
    .uartHandle = (ns_uart_handle_t)&rpcGenericUARTHandle, // we temporarily set the uartHandle here to allow the user to set the blocking/nonblocking send/receive in the uart transport layer
#endif        
    .sendBlockToEVB_cb = NULL,
    .fetchBlockFromEVB_cb = NULL,
    .computeOnEVB_cb = NULL,
#ifdef NS_USB_PRESENT
    .transport = NS_RPC_TRANSPORT_USB};
#else
    .transport = NS_RPC_TRANSPORT_UART};
#endif

static dataBlock resultBlock;
#include "ref_f2m.h"
__attribute__((section(".sram_bss"))) int16_t mel[TIMESTEPS * 300];
__attribute__((section(".sram_bss"))) int32_t wavout[TIMESTEPS * 256];

int main(void) {
    ns_core_config_t core_cfg = {.api = &ns_core_V1_0_0};
    NS_TRY(ns_core_init(&core_cfg), "Core init failed");
    NS_TRY(ns_power_config(&ns_development_default), "Power Init Failed");
    NS_TRY(ns_peripheral_button_init(&button_config), "Button init failed\n");
    NS_TRY(ns_rpc_genericDataOperations_init(&rpcConfig), "RPC Init Failed\n"); // init RPC and USB
    ns_itm_printf_enable();

    tts_init();

    // ns_lp_printf("phone2fuse\n");
    // unitTest_phone2fuse();

    // ns_lp_printf("fuse2mel\n");
    // unitTest_fuse2mel();

    ns_printf("Start the PC-side server, then press Button 0 to get started\n");
    while (g_intButtonPressed == 0) {
        ns_delay_us(1000);
    }

    tts(mel);
    // int16_t *ptr = (int16_t*) mel; // mel, ref_f2m_output
    // for (int i = 0 ; i < 300; i++) {
    //     for (int j = 0 ; j < 80; j++) {
    //         audioDataBuffer[j] = ptr[i * 80 + j];
            
    //     }
    //     ns_rpc_data_sendBlockToPC(&outBlock);
    // }
    GRIFFIN_LIN_algorithm_frame(wavout);


    int32_t *ptr = (int32_t*) wavout; // mel, ref_f2m_output
    for (int i = 0 ; i < 300; i++) {
        for (int j = 0 ; j < 256; j++) {
            audioDataBuffer[j] = ptr[i * 256 + j];
            
        }
        ns_rpc_data_sendBlockToPC(&outBlock);
        ns_lp_printf("Sent block %d to PC\n", i);
    }

    return 0;   
}