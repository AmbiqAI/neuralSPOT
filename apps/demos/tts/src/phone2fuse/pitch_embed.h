#ifndef PITCH_EMBED_H
#define PITCH_EMBED_H

#include <stdint.h>
#define NUM_EMBEDS_PITCH 32
#define DIM_FEAT_PITCH 32
#define NUM_BINS_PITCH 31
extern int16_t pitch_embed[1024];
extern float pitch_bins[31];
#endif
