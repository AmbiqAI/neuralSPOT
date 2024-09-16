#ifndef __DEBUG_FILES_H__
#define __DEBUG_FILES_H__
#include "ambiq_nnsp_debug.h"
#if AMBIQ_NNSP_DEBUG == 1
    #include <stdio.h>
extern FILE *file_spec_c;
extern FILE *file_pspec_c;
extern FILE *file_feat_c;
extern FILE *file_melSpec_c;
extern FILE *file_mask_c;

void open_debug_files();
void close_debug_files();
#endif
#endif
