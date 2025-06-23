#include "model_upload.h"
#include "am_util_stdio.h"
#include "FreeRTOS.h"
#include "task.h"
#include "seCntrlClass.h"
#include <cstring>  // for memcpy

static model_upload_state_t upload_state = {0};

void model_upload_init(void) {
    upload_state.total_chunks = 0;
    upload_state.received_chunks = 0;
    upload_state.model_buffer = NULL;
    upload_state.model_size = 0;
    upload_state.upload_in_progress = false;
}

bool handle_model_chunk(const uint8_t* data, uint32_t length, uint32_t chunk_num, uint8_t total_chunks) {
    // First chunk - initialize upload
    if (chunk_num == 0) {
        if (upload_state.upload_in_progress) {
            am_util_stdio_printf("Error: Model upload already in progress\n");
            return false;
        }
        
        // Calculate total model size
        upload_state.total_chunks = total_chunks;
        upload_state.model_size = total_chunks * MAX_CHUNK_SIZE;
        
        // Allocate buffer for complete model
        upload_state.model_buffer = static_cast<uint8_t*>(pvPortMalloc(upload_state.model_size));
        if (!upload_state.model_buffer) {
            am_util_stdio_printf("Error: Failed to allocate model buffer\n");
            return false;
        }
        
        upload_state.upload_in_progress = true;
        upload_state.received_chunks = 0;
    }
    
    // Validate chunk number
    if (chunk_num >= total_chunks) {
        am_util_stdio_printf("Error: Invalid chunk number %d (total: %d)\n", chunk_num, total_chunks);
        return false;
    }
    
    // Copy chunk data to buffer
    uint32_t offset = chunk_num * MAX_CHUNK_SIZE;
    memcpy(upload_state.model_buffer + offset, data, length);
    upload_state.received_chunks++;
    
    // Check if upload is complete
    if (upload_state.received_chunks == total_chunks) {
        am_util_stdio_printf("Model upload complete. Size: %d bytes\n", upload_state.model_size);
        
        // Initialize model with uploaded data
        extern seCntrlClass cntrl_inst;  // Reference to global instance
        seCntrlClass_init(&cntrl_inst);  // Initialize the instance
        
        // TODO: Add model data loading after initialization
        // This will need to be implemented in seCntrlClass
        
        am_util_stdio_printf("Model initialized successfully\n");
        upload_state.upload_in_progress = false;
        return true;
    }
    
    return true;
}

model_upload_state_t* get_model_upload_state(void) {
    return &upload_state;
}

void model_upload_cleanup(void) {
    if (upload_state.model_buffer) {
        vPortFree(upload_state.model_buffer);
        upload_state.model_buffer = NULL;
    }
    upload_state.upload_in_progress = false;
    upload_state.total_chunks = 0;
    upload_state.received_chunks = 0;
    upload_state.model_size = 0;
} 