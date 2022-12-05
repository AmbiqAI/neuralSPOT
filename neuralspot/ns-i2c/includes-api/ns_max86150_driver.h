/**
 * @file ns_max86150_driver.h
 * @author Adam Page (adam.page@ambiq.com)
 * @brief Register driver for MAX86150 ECG/PPG IC
 * @version 0.1
 * @date 2022-11-02
 *
 * @copyright Copyright (c) 2022
 *
 *  \addtogroup NeuralSPOT-MAX86150
 *  @{
 *  @ingroup NeuralSPOT-i2c
 */
#ifndef NS_MAX86150_H
#define NS_MAX86150_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ns_i2c.h"
#include <stddef.h>
#include <stdint.h>

typedef enum {
    Max86150SlotOff = 0,
    Max86150SlotPpgLed1 = 1,
    Max86150SlotPpgLed2 = 2,
    Max86150SlotPilotLed1 = 5,
    Max86150SlotPilotLed2 = 6,
    Max86150SlotEcg = 9
} max86150_slot_type;

static const uint8_t MAX86150_FIFO_DEPTH = 32;

typedef int (*pfnI2cWriteRead)(uint16_t addr, const void *write_buf, size_t num_write,
                               void *read_buf, size_t num_read);
typedef int (*pfnI2cRead)(const void *buf, uint32_t num_bytes, uint16_t addr);
typedef int (*pfnI2cWrite)(const void *buf, uint32_t num_bytes, uint16_t addr);

typedef struct {
    uint32_t addr;
    pfnI2cWriteRead i2c_write_read;
    pfnI2cRead i2c_read;
    pfnI2cWrite i2c_write;
} max86150_context_t;

uint8_t
max86150_get_register(const max86150_context_t *ctx, uint8_t reg, uint8_t mask);
int
max86150_set_register(const max86150_context_t *ctx, uint8_t reg, uint8_t value, uint8_t mask);

// Interrupts
uint8_t
max86150_get_int1(const max86150_context_t *ctx);
uint8_t
max86150_get_int2(const max86150_context_t *ctx);

void
max86150_set_alm_full_int_flag(const max86150_context_t *ctx, uint8_t enable);
void
max86150_set_data_rdy_int_flag(const max86150_context_t *ctx, uint8_t enable);
void
max86150_set_alc_ovf_int_flag(const max86150_context_t *ctx, uint8_t enable);
void
max86150_set_prox_int_flag(const max86150_context_t *ctx, uint8_t enable);
void
max86150_set_vdd_oor_int_flag(const max86150_context_t *ctx, uint8_t enable);
void
max86150_set_ecg_rdy_int_flag(const max86150_context_t *ctx, uint8_t enable);

uint8_t
max86150_get_fifo_wr_pointer(const max86150_context_t *ctx);
void
max86150_set_fifo_wr_pointer(const max86150_context_t *ctx, uint8_t value);
void
max86150_set_fifo_slot(const max86150_context_t *ctx, uint8_t slot, max86150_slot_type type);
void
max86150_set_fifo_slots(const max86150_context_t *ctx, max86150_slot_type *slots);
void
max86150_disable_slots(const max86150_context_t *ctx);
uint32_t
max86150_read_fifo_samples(const max86150_context_t *ctx, uint32_t *buffer,
                           max86150_slot_type *slots, uint8_t numSlots);
uint8_t
max86150_get_fifo_overflow_counter(const max86150_context_t *ctx);
uint8_t
max86150_set_fifo_overflow_counter(const max86150_context_t *ctx, uint8_t value);
uint8_t
max86150_get_fifo_rd_pointer(const max86150_context_t *ctx);
void
max86150_set_fifo_rd_pointer(const max86150_context_t *ctx, uint8_t value);

void
max86150_set_almost_full_int_options(const max86150_context_t *ctx, uint8_t options);
void
max86150_set_almost_full_flag_options(const max86150_context_t *ctx, uint8_t options);
void
max86150_set_almost_full_rollover(const max86150_context_t *ctx, uint8_t enable);
void
max86150_set_almost_full_threshold(const max86150_context_t *ctx, uint8_t space);
void
max86150_set_fifo_enable(const max86150_context_t *ctx, uint8_t enable);

void
max86150_powerup(const max86150_context_t *ctx);
void
max86150_shutdown(const max86150_context_t *ctx);
void
max86150_reset(const max86150_context_t *ctx);

void
max86150_set_ppg_adc_range(const max86150_context_t *ctx, uint8_t range);
void
max86150_set_ppg_sample_rate(const max86150_context_t *ctx, uint8_t value);
void
max86150_set_ppg_pulse_width(const max86150_context_t *ctx, uint8_t value);
void
max86150_set_ppg_sample_average(const max86150_context_t *ctx, uint8_t value);
void
max86150_set_proximity_threshold(const max86150_context_t *ctx, uint8_t value);
void
max86150_set_led_pulse_amplitude(const max86150_context_t *ctx, uint8_t led, uint8_t value);
void
max86150_set_led_current_range(const max86150_context_t *ctx, uint8_t led, uint8_t value);
void
max86150_set_ecg_sample_rate(const max86150_context_t *ctx, uint8_t value);
void
max86150_set_ecg_pga_gain(const max86150_context_t *ctx, uint8_t value);
void
max86150_set_ecg_ia_gain(const max86150_context_t *ctx, uint8_t value);
uint8_t
max86150_get_part_id(const max86150_context_t *ctx);

void
max86150_clear_fifo(const max86150_context_t *ctx);

#ifdef __cplusplus
}
#endif
/** @}*/
#endif // NS_MAX86150_H
