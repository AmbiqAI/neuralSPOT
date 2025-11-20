#ifndef OE_API__H
#define OE_API__H

int oe_init(void);
int oe_encode_frame(short *p_pcm_buffer, int n_pcm_samples, char *p_encoded_buffer);

#endif // OE_API__H


