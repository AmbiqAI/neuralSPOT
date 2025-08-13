#ifndef str_ww_ref_model_model_h
#define str_ww_ref_model_model_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define str_ww_ref_model_num_inputs 1
#define str_ww_ref_model_num_outputs 1

#define str_ww_ref_model_input_0_size (1200)

#define str_ww_ref_model_output_0_size (3)

extern const int32_t str_ww_ref_model_inputs_len[str_ww_ref_model_num_inputs];
extern const int32_t str_ww_ref_model_inputs_zero_point[str_ww_ref_model_num_inputs];
extern const float str_ww_ref_model_inputs_scale[str_ww_ref_model_num_inputs];
extern const int32_t str_ww_ref_model_outputs_len[str_ww_ref_model_num_outputs];
extern const int32_t str_ww_ref_model_outputs_zero_point[str_ww_ref_model_num_outputs];
extern const float str_ww_ref_model_outputs_scale[str_ww_ref_model_num_outputs];


/// Operator states passed to the callback
typedef enum {
    str_ww_ref_model_model_state_started = 0,  // <-- operator started
    str_ww_ref_model_model_state_finished,     // <-- operator finished
} str_ww_ref_model_operator_state_e;

/// Signature for the operator callback
typedef void (*str_ww_ref_model_operator_callback)(
    int32_t              op,           // <-- operator identifier
    str_ww_ref_model_operator_state_e state,  // <-- operator state
    int32_t              status,       // <-- return code
    void                *user_data     // <-- opaque user data
);

/// Holds the callback and opaque user context
typedef struct {
    const void* input_data[str_ww_ref_model_num_inputs];
    int32_t input_len[str_ww_ref_model_num_inputs];

    void* output_data[str_ww_ref_model_num_outputs];
    int32_t output_len[str_ww_ref_model_num_outputs];

    str_ww_ref_model_operator_callback callback;
    void *user_data;
} str_ww_ref_model_model_context_t;


int32_t str_ww_ref_model_model_init(str_ww_ref_model_model_context_t *context);

int32_t str_ww_ref_model_model_run(str_ww_ref_model_model_context_t *context);

#ifdef __cplusplus
}
#endif

#endif // str_ww_ref_model_model_h