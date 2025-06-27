#ifndef __MINMAX_H__
#define __MINMAX_H__
#ifdef __cplusplus
extern "C" {
#endif
#ifndef MAX
#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif
#ifdef __cplusplus
}
#endif
#endif
