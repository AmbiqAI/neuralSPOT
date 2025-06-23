#ifndef MODEL_UPLOAD_H
#define MODEL_UPLOAD_H

#include <stdint.h>
#include "usb_packet_generated.h"

// Maximum size of a single chunk
#define MAX_CHUNK_SIZE 1024

// Structure to track model upload state
typedef struct {
    uint32_t total_chunks;
    uint32_t received_chunks;
    uint8_t* model_buffer;
    uint32_t model_size;
    bool upload_in_progress;
} model_upload_state_t;

// Initialize model upload state
void model_upload_init(void);

// Handle incoming model data chunk
bool handle_model_chunk(const uint8_t* data, uint32_t length, uint32_t chunk_num, uint8_t total_chunks);

// Get current model upload state
model_upload_state_t* get_model_upload_state(void);

// Clean up model upload resources
void model_upload_cleanup(void);

#endif // MODEL_UPLOAD_H 