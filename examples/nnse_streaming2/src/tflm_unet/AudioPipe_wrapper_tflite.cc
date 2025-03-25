#include <cstdlib>
#include <cstring>
#include "../def_nnse_params.h"
#include "mut_model_metadata.h"
#include "mut_model_data.h"
#include "tflm_ns_model.h"
#include <stdint.h>
#include "../AudioPipe_wrapper.h"

#include "feature_module.h"
#include "ns_ambiqsuite_harness.h"
#include "nn_speech.h"
#include "iir.h"


// Feature class instance
FeatureClass FEAT_INST;
IIR_CLASS dcrm_inst;
// normalized feature parameters of NNSE:
const int32_t feature_mean_se1[] = {-86475, -64400, -51399, -43912, -38203, -37334, -35035, -35706, -38982, -42105, -43979, -44918, -45177, -45401, -46249, -47396, -48354, -49945, -51595, -53045, -55516, -57004, -58980, -60901, -62621, -63733, -65190, -66989, -67750, -69049, -70815, -71217, -72014, -72935, -73705, -74695, -75330, -75137, -75288, -76762, -77036, -77611, -76717, -77480, -78061, -78052, -78665, -77869, -78024, -78698, -77841, -78752, -79858, -79724, -80688, -81247, -80317, -80990, -82613, -82581, -82920, -84024, -84166, -83753, -84491, -85776, -85787, -85662, -86438, -87160, -87010, -86865, -87638, -88203, -87772, -87088, -87282, -88185, -88302, -87955, -87940, -88545, -89024, -88934, -88509, -88742, -89735, -90340, -90331, -90285, -90490, -91174, -91644, -91801, -91636, -91666, -92169, -92761, -92865, -92773, -92705, -92888, -93560, -94140, -94319, -94302, -94397, -94810, -95432, -95835, -95982, -95923, -95877, -95986, -96555, -97172, -97499, -97693, -97830, -97828, -98158, -98697, -99053, -99285, -99380, -99261, -99163, -99319, -99835, -100482, -100863, -100878, -100978, -101270, -101419, -101786, -102300, -102739, -102941, -103159, -103396, -103584, -103756, -104227, -104727, -105069, -105301, -105454, -105605, -105805, -105800, -105916, -106234, -106617, -107003, -107183, -107373, -107658, -107801, -107940, -108183, -108637, -108939, -109177, -109299, -109451, -109527, -109623, -109766, -109791, -109998, -110347, -110582, -110676, -110867, -110974, -111005, -111195, -111346, -111373, -111504, -111754, -111965, -112052, -112165, -112344, -112392, -112376, -112453, -112470, -112542, -112694, -112818, -113100, -113185, -113242, -113390, -113582, -113646, -113604, -113506, -113491, -113657, -113832, -114036, -114238, -114340, -114368, -114463, -114570, -114682, -114738, -114885, -114945, -115099, -115220, -115394, -115523, -115577, -115632, -115749, -115789, -115850, -115930, -115959, -116102, -116124, -116208, -116316, -116458, -116654, -116818, -116793, -116946, -117162, -117358, -117571, -117948, -118436, -119016, -119704, -120676, -121856, -123228, -124888, -126651, -128517, -130345, -132083, -133557, -134712, -135493, -135943, -136229, -136461, -136683, -138024, };
const int32_t feature_stdR_se1[] = {24194, 21529, 20558, 19855, 19531, 19643, 19467, 19481, 19764, 20051, 20184, 20142, 20058, 19934, 19894, 19989, 20063, 20164, 20308, 20369, 20506, 20628, 20793, 20943, 21115, 21242, 21501, 21676, 21780, 22003, 22092, 22237, 22343, 22383, 22345, 22499, 22614, 22582, 22538, 22747, 22738, 22871, 22769, 22910, 22949, 22889, 23093, 22978, 22977, 23078, 22944, 23096, 23293, 23251, 23356, 23457, 23344, 23455, 23677, 23697, 23725, 23943, 24007, 23963, 24096, 24246, 24204, 24109, 24237, 24355, 24352, 24348, 24477, 24585, 24530, 24355, 24364, 24537, 24491, 24442, 24416, 24526, 24584, 24547, 24470, 24539, 24679, 24734, 24767, 24771, 24780, 24907, 25008, 24987, 24944, 24978, 25094, 25135, 25148, 25147, 25117, 25144, 25308, 25454, 25430, 25422, 25421, 25484, 25619, 25654, 25649, 25611, 25613, 25639, 25763, 25864, 25918, 25952, 25988, 25940, 25966, 26087, 26151, 26177, 26182, 26153, 26129, 26132, 26243, 26314, 26341, 26298, 26296, 26354, 26358, 26402, 26514, 26644, 26654, 26630, 26695, 26744, 26772, 26850, 26952, 26967, 26970, 27013, 27055, 27116, 27094, 27064, 27112, 27174, 27268, 27270, 27280, 27358, 27388, 27428, 27522, 27589, 27644, 27684, 27714, 27766, 27750, 27749, 27803, 27810, 27845, 27891, 27915, 27904, 27967, 28031, 28031, 28103, 28145, 28141, 28188, 28258, 28309, 28305, 28342, 28430, 28409, 28392, 28485, 28498, 28502, 28539, 28639, 28684, 28653, 28706, 28759, 28860, 28923, 28894, 28837, 28781, 28895, 28915, 29046, 29169, 29228, 29219, 29207, 29296, 29406, 29437, 29478, 29485, 29533, 29518, 29601, 29653, 29644, 29698, 29742, 29778, 29799, 29872, 29923, 29953, 29954, 29941, 29956, 29970, 30044, 30174, 30127, 30161, 30253, 30348, 30403, 30549, 30766, 31008, 31245, 31672, 32294, 33026, 34054, 35203, 36587, 38073, 39583, 40861, 41828, 42394, 42702, 43005, 43360, 43791, 46179, };

// TFLM Config
static ns_model_state_t tflm;

// TF Tensor Arena

#if (TFLM_MODEL_LOCATION == NS_AD_PSRAM)
    unsigned char *mut_model;
#endif

#if (TFLM_ARENA_LOCATION == NS_AD_PSRAM)
    static uint8_t *tensor_arena;
    static constexpr int kTensorArenaSize = 1024 * 1024 * 10; // 10MB
#else
    static constexpr int kTensorArenaSize = 1024 * TFLM_VALIDATOR_ARENA_SIZE;
    // #ifdef AM_PART_APOLLO3
    //     // Apollo3 doesn't have AM_SHARED_RW
    //     alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
    // #else // not AM_PART_APOLLO3
        #if (TFLM_ARENA_LOCATION == NS_AD_SRAM)
            #ifdef keil6
            // Align to 16 bytes
            AM_SHARED_RW __attribute__((aligned(16))) static uint8_t tensor_arena[kTensorArenaSize];
            #else
            AM_SHARED_RW alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
            #endif
        #else
            NS_PUT_IN_TCM alignas(16) static uint8_t tensor_arena[kTensorArenaSize];
        #endif
    // #endif
#endif

// Resource Variable Arena - always in TCM for now
static constexpr int kVarArenaSize = 4096;
    // 4 * (TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES + 1) * sizeof(tflite::MicroResourceVariables);
alignas(16) static uint8_t var_arena[kVarArenaSize];
// Validator Stuff

volatile int example_status = 0; // Prevent the compiler from optimizing out while loops

extern const int16_t stft_win_coeff_w480_h160[];
int AudioPipe_wrapper_init(void)
{ 
    FeatureClass_construct(
        &FEAT_INST,
        (const int32_t*) feature_mean_se1,
        (const int32_t*) feature_stdR_se1,
        FEATURE_QBIT,
        257, // in unet, we use Pspectrum as input directly
        FEATURE_WINSIZE,
        FEATURE_HOPSIZE,
        FEATURE_FFTSIZE,
        stft_win_coeff_w480_h160);

    IIR_CLASS_init(&dcrm_inst);
    
    // Initialize the model, get handle if successful
    
    tflm.runtime = TFLM;
    tflm.model_array = mut_model;
    tflm.arena = tensor_arena;
    tflm.arena_size = kTensorArenaSize;
    tflm.rv_arena = var_arena;
    tflm.rv_arena_size = kVarArenaSize;
    tflm.rv_count = TFLM_VALIDATOR_MAX_RESOURCE_VARIABLES;
    tflm.numInputTensors = 1;
    tflm.numOutputTensors = 1;

    int status = tflm_validator_model_init(&tflm); // model init with minimal defaults

    if (status == NS_STATUS_FAILURE) {
        while (1)
            example_status = NS_STATUS_INIT_FAILED; // hang
    }

    ns_lp_printf("Model initialized\n");
    return 0;
}

int AudioPipe_wrapper_reset(void)
{
    FeatureClass_setDefault(&FEAT_INST);
    IIR_CLASS_reset(&dcrm_inst);
    return 0;
}

int AudioPipe_wrapper_frameProc(
        int16_t *pcm_input,
        int16_t *pcm_output)
{
    /* feature extraction
    1. iir for dc remove
    2. melspectrogram
    */

    static int16_t tmp_16s[300];
    static float scalar_norm = 1.0 / (float) (1 << FEATURE_QBIT);
    IIR_CLASS_exec(&dcrm_inst, tmp_16s, pcm_input, FEATURE_HOPSIZE);
    FeatureClass_execute(&FEAT_INST, tmp_16s);

    int16_t *ptfeat = FEAT_INST.normFeatContext + FEATURE_NUM_MFC * (FEATURE_CONTEXT-1);

    float input_scale = tflm.model_input[0]->params.scale;
    int input_zero_point = tflm.model_input[0]->params.zero_point;    

    for (int i =0; i < FEATURE_NUM_MFC; i++)
    {
        float val = ((float) ptfeat[i] ) * scalar_norm;
        int16_t input = (int16_t) ((float32_t) val / (float32_t) input_scale + (float32_t) input_zero_point);
        tflm.model_input[0]->data.i16[i] =  input;
    }

    TfLiteStatus invoke_status = tflm.interpreter->Invoke(); 
    if (invoke_status != kTfLiteOk) {
        while (1)
        {
            example_status = NS_STATUS_FAILURE; // invoke failed, so hang
        }
    }
    float output_scale = tflm.model_output[0]->params.scale;
    int output_zero_point = tflm.model_output[0]->params.zero_point;
    
    for (int i = 0; i < NN_DIM_OUT; i++) {
        float32_t out; 
        out = (float32_t) (tflm.model_output[0]->data.i16[i] - output_zero_point);
        out = out * output_scale;
        
        // ns_lp_printf("%f ", out);
        tmp_16s[i] = (int16_t) (out * 32768);
    }
    // ns_lp_printf("\n");
    // // get the tf mask
    se_post_proc(
        &FEAT_INST,
        tmp_16s,
        pcm_output,
        0,
        NN_DIM_OUT);
    return 0;
}

