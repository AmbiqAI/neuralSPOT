#include "nnid_class.h"
#include "nn_speech.h"
#include <math.h>
#include "ambiq_nnsp_debug.h"

#if DEBUG_NNID
int32_t embd[] = {-30250, -8811, -16640, 16746,  15859,  -58364, -9399,  10272,  -18910, 68483,
                  8228,   33163, -15106, -5028,  -14175, 62699,  2091,   9407,   5148,   -44625,
                  -16550, 43527, -43629, -13265, -41046, -24585, -13471, 69923,  -15037, 57163,
                  2441,   -8800, -7792,  -15368, -19536, 12774,  -2713,  -67111, 10742,  -12631,
                  33019,  -265,  -22142, -9418,  16120,  28853,  30603,  32016,  -53020, -68356,
                  -15268, -7651, 25368,  -27267, 3024,   12461,  36765,  45967,  7833,   -37081,
                  -3423,  6051,  -37274, 34973};
#endif

void nnidClass_get_cos(int32_t *pt_nn_est, int32_t *pt_embd, int16_t len, float *corr) {
    int i;
    float acc = 0;
    float norm1, norm2;
    float tmp;
#if DEBUG_NNID
    pt_nn_est = embd;
#endif
    for (i = 0; i < len; i++)
        acc += (3.0518e-05 * (float)pt_nn_est[i]) * (3.0518e-05 * (float)pt_embd[i]);

    norm1 = 0;
    for (i = 0; i < len; i++) {
        tmp = 3.0518e-05 * (float)pt_nn_est[i];
        norm1 += tmp * tmp;
    }

    norm2 = 0;
    for (i = 0; i < len; i++) {
        tmp = 3.0518e-05 * (float)pt_embd[i];
        norm2 += tmp * tmp;
    }
    acc /= sqrtf((float)(norm1 * norm2));
    *corr = acc;
}

void nnidClass_reset_states(NNID_CLASS *pt_nnid) {
    pt_nnid->is_get_corr = 0;
    pt_nnid->corr = 0;
}
