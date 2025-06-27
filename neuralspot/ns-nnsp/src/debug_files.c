#include "ambiq_nnsp_debug.h"
#if AMBIQ_NNSP_DEBUG == 1
    #include <stdio.h>
FILE *file_spec_c;
FILE *file_pspec_c;
FILE *file_feat_c;
FILE *file_melSpec_c;
FILE *file_mask_c;

void open_debug_files() {
    file_spec_c = fopen("dump/file_spec_c", "w");
    file_pspec_c = fopen("dump/file_pspec_c", "w");
    file_feat_c = fopen("dump/file_feat_c", "w");
    file_melSpec_c = fopen("dump/file_melSpec_c", "w");
    file_mask_c = fopen("dump/file_mask_c", "w");
}

void close_debug_files() {
    fclose(file_spec_c);
    fclose(file_pspec_c);
    fclose(file_feat_c);
    fclose(file_melSpec_c);
    fclose(file_mask_c);
}
#endif
