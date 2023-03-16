#ifndef NS_PORTING_HARNESS_H
#define NS_PORTING_HARNESS_H

#ifdef __cplusplus
extern "C" {
#endif

extern void
ns_min_printf(const char *format, ...);

extern void
ns_min_enable_printf(void);

extern void
ns_min_enable_timer(void);

extern unsigned long
ns_min_get_timer_us(void);

#ifdef __cplusplus
}
#endif

#endif
