#ifndef str_ww_ref_model_test_case_h
#define str_ww_ref_model_test_case_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Initialize unit test
 *
 * @return 0 on SUCCESS
 */
int32_t str_ww_ref_model_test_case_init(void);


/**
 * @brief Run the unit test
 *
 * @return 0 on SUCCESS
 */
int32_t str_ww_ref_model_test_case_run(void);

#ifdef __cplusplus
}
#endif

#endif // str_ww_ref_model_test_case_h