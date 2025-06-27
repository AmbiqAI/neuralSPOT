

# File ns\_max86150\_driver.c

[**File List**](files.md) **>** [**neuralSPOT**](dir_75594cce7c7773aa3cb253214bf56510.md) **>** [**neuralspot**](dir_b737d82f35ec218ac5a7ef4105db9c0e.md) **>** [**ns-i2c**](dir_cc1f492d5d9f73ec0b0ac0581cc456e0.md) **>** [**src**](dir_d6c56226b0c7cd8d6baff594b5865597.md) **>** [**ns\_max86150\_driver.c**](ns__max86150__driver_8c.md)

[Go to the documentation of this file](ns__max86150__driver_8c.md)


```C++

#include "ns_max86150_driver.h"
#include <ctype.h>
#include <stdint.h>
#include <string.h>

// Status Registers
static const uint8_t MAX86150_INT_STAT1 = 0x00;
static const uint8_t MAX86150_INT_STAT2 = 0x01;
static const uint8_t MAX86150_INT_EN1 = 0x02;
static const uint8_t MAX86150_INT_EN2 = 0x03;
// FIFO Registers
static const uint8_t MAX86150_FIFO_WR_PTR = 0x04;
static const uint8_t MAX86150_FIFO_OVERFLOW = 0x05;
static const uint8_t MAX86150_FIFO_RD_PTR = 0x06;
static const uint8_t MAX86150_FIFO_DATA = 0x07;
static const uint8_t MAX86150_FIFO_CONFIG = 0x08;
// FIFO Data Control
static const uint8_t MAX86150_FIFO_CONTROL1 = 0x09;
static const uint8_t MAX86150_FIFO_CONTROL2 = 0x0A;
// System Control
static const uint8_t MAX86150_SYS_CONTROL = 0x0D;
// PPG Configuration
static const uint8_t MAX86150_PPG_CONFIG1 = 0x0E;
static const uint8_t MAX86150_PPG_CONFIG2 = 0x0F;
static const uint8_t MAX86150_PPG_PROX_INT_THRESH = 0x10;
// LED Pulse Amplitude
static const uint8_t MAX86150_LED1_PA = 0x11;
static const uint8_t MAX86150_LED2_PA = 0x12;
static const uint8_t MAX86150_LED_RANGE = 0x14;
static const uint8_t MAX86150_LEDP_PA = 0x15;
// ECG Configuration
static const uint8_t MAX86150_ECG_CONFIG1 = 0x3C;
static const uint8_t MAX86150_ECG_CONFIG3 = 0x3E;
// Part ID
static const uint8_t MAX86150_PART_ID = 0xFF;
// static const uint8_t MAX86150_PART_ID_VAL = 0x1E;

uint8_t
max86150_get_register(const max86150_context_t *ctx, uint8_t reg, uint8_t mask) {
    uint32_t regAddr = reg;
    uint32_t regVal;
    uint8_t value;
    ctx->i2c_write_read(ctx->addr, &regAddr, 1, &regVal, 1);
    value = regVal & 0x00FF;
    if (mask != 0xFF) {
        value &= mask;
    }
    return value;
}
int
max86150_set_register(const max86150_context_t *ctx, uint8_t reg, uint8_t value, uint8_t mask) {
    int err;
    uint8_t i2cBuffer[4];
    if (mask != 0xFF) {
        value = max86150_get_register(ctx, reg, ~mask) | (value & mask);
    }
    i2cBuffer[0] = reg;
    i2cBuffer[1] = value;
    err = ctx->i2c_write(&(i2cBuffer[0]), 2, ctx->addr);
    return err;
}

// Status Registers
uint8_t
max86150_get_int1(const max86150_context_t *ctx) {
    return max86150_get_register(ctx, MAX86150_INT_STAT1, 0xFF);
}

uint8_t
max86150_get_int2(const max86150_context_t *ctx) {
    return max86150_get_register(ctx, MAX86150_INT_STAT2, 0xFF);
}

void
max86150_set_alm_full_int_flag(const max86150_context_t *ctx, uint8_t enable) {
    max86150_set_register(ctx, MAX86150_INT_EN1, enable << 7, 0x80);
}

void
max86150_set_ppg_rdy_int_flag(const max86150_context_t *ctx, uint8_t enable) {
    max86150_set_register(ctx, MAX86150_INT_EN1, enable << 6, 0x40);
}

void
max86150_set_alc_ovf_int_flag(const max86150_context_t *ctx, uint8_t enable) {
    max86150_set_register(ctx, MAX86150_INT_EN1, enable << 5, 0x20);
}

void
max86150_set_prox_int_flag(const max86150_context_t *ctx, uint8_t enable) {
    max86150_set_register(ctx, MAX86150_INT_EN1, enable << 4, 0x10);
}

void
max86150_set_vdd_oor_int_flag(const max86150_context_t *ctx, uint8_t enable) {
    max86150_set_register(ctx, MAX86150_INT_EN2, enable << 7, 0x80);
}

void
max86150_set_ecg_rdy_int_flag(const max86150_context_t *ctx, uint8_t enable) {
    max86150_set_register(ctx, MAX86150_INT_EN2, enable << 2, 0x04);
}

// FIFO Registers

uint8_t
max86150_get_fifo_wr_pointer(const max86150_context_t *ctx) {
    return max86150_get_register(ctx, MAX86150_FIFO_WR_PTR, 0x1F);
}

void
max86150_set_fifo_wr_pointer(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_FIFO_WR_PTR, value, 0x1F);
}

void
max86150_set_fifo_slot(const max86150_context_t *ctx, uint8_t slot, max86150_slot_type type) {
    uint8_t reg = slot & 0x02 ? MAX86150_FIFO_CONTROL2 : MAX86150_FIFO_CONTROL1;
    uint8_t value = slot & 0x01 ? type << 4 : type;
    uint8_t mask = slot & 0x01 ? 0xF0 : 0x0F;
    max86150_set_register(ctx, reg, value, mask);
}

void
max86150_set_fifo_slots(const max86150_context_t *ctx, max86150_slot_type *slots) {
    for (size_t i = 0; i < 4; i++) {
        max86150_set_fifo_slot(ctx, i, slots[i]);
    }
}

void
max86150_disable_slots(const max86150_context_t *ctx) {
    max86150_set_register(ctx, MAX86150_FIFO_CONTROL1, 0x00, 0xFF);
    max86150_set_register(ctx, MAX86150_FIFO_CONTROL2, 0x00, 0xFF);
}

uint32_t
max86150_read_fifo_samples(const max86150_context_t *ctx, uint32_t *buffer,
                           max86150_slot_type *slots, uint8_t numSlots) {
    uint8_t temp[4];
    uint8_t rdBytes[3 * 4];
    uint32_t rdBytesIdx;
    uint32_t bufferIdx;
    uint32_t regAddr = MAX86150_FIFO_WR_PTR;
    ctx->i2c_write_read(ctx->addr, &regAddr, 1, temp, 3);
    uint8_t wrPtr = temp[0] & 0x1F;
    uint8_t ovrCnt = temp[1] & 0x1F;
    uint8_t rdPtr = temp[2] & 0x1F;
    uint32_t bytesPerSample = 3 * numSlots;
    uint32_t numSamples;
    // If overflow or pointers are equal, then FIFO is full
    // NOTE: When FIFO cleared wr=rd=ovr=0 not sure how to handle this
    if (ovrCnt || (wrPtr && (rdPtr == wrPtr))) {
        numSamples = MAX86150_FIFO_DEPTH;
    } else if (wrPtr > rdPtr) {
        numSamples = wrPtr - rdPtr;
    } else {
        numSamples = MAX86150_FIFO_DEPTH + wrPtr - rdPtr;
    }
    if (numSamples == 0) {
        return numSamples;
    }
    regAddr = MAX86150_FIFO_DATA;
    ctx->i2c_write(&regAddr, 1, ctx->addr);
    bufferIdx = 0;
    for (size_t i = 0; i < numSamples; i++) {
        ctx->i2c_read((void *)rdBytes, bytesPerSample, ctx->addr);
        rdBytesIdx = 0;
        for (size_t j = 0; j < numSlots; j++) {
            temp[3] = 0;
            temp[2] = rdBytes[rdBytesIdx++];
            temp[1] = rdBytes[rdBytesIdx++];
            temp[0] = rdBytes[rdBytesIdx++];
            memcpy(&buffer[bufferIdx], temp, 4);
            if (slots[j] == Max86150SlotEcg) {
                buffer[bufferIdx] &= 0x3FFFF;
            } else {
                buffer[bufferIdx] &= 0x7FFFF;
            }
            bufferIdx += 1;
        }
    }
    return numSamples;
}

uint8_t
max86150_get_fifo_overflow_counter(const max86150_context_t *ctx) {
    return max86150_get_register(ctx, MAX86150_FIFO_OVERFLOW, 0x1F);
}

uint8_t
max86150_set_fifo_overflow_counter(const max86150_context_t *ctx, uint8_t value) {
    return max86150_set_register(ctx, MAX86150_FIFO_OVERFLOW, value, 0x1F);
}

uint8_t
max86150_get_fifo_rd_pointer(const max86150_context_t *ctx) {
    return max86150_get_register(ctx, MAX86150_FIFO_RD_PTR, 0x1F);
}

void
max86150_set_fifo_rd_pointer(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_FIFO_RD_PTR, value, 0x1F);
}

void
max86150_set_almost_full_int_options(const max86150_context_t *ctx, uint8_t options) {
    max86150_set_register(ctx, MAX86150_FIFO_CONFIG, options << 6, 0x40);
}

void
max86150_set_almost_full_flag_options(const max86150_context_t *ctx, uint8_t options) {
    max86150_set_register(ctx, MAX86150_FIFO_CONFIG, options << 5, 0x20);
}

void
max86150_set_almost_full_rollover(const max86150_context_t *ctx, uint8_t enable) {
    max86150_set_register(ctx, MAX86150_FIFO_CONFIG, enable << 4, 0x10);
}

void
max86150_set_almost_full_threshold(const max86150_context_t *ctx, uint8_t space) {
    max86150_set_register(ctx, MAX86150_FIFO_CONFIG, space, 0x0F);
}

// System Registers

void
max86150_set_fifo_enable(const max86150_context_t *ctx, uint8_t enable) {
    max86150_set_register(ctx, MAX86150_SYS_CONTROL, enable << 2, 0x4);
}

void
max86150_shutdown(const max86150_context_t *ctx) {
    max86150_set_register(ctx, MAX86150_SYS_CONTROL, 0x2, 0x2);
}

void
max86150_powerup(const max86150_context_t *ctx) {
    max86150_set_register(ctx, MAX86150_SYS_CONTROL, 0x0, 0x2);
}

void
max86150_reset(const max86150_context_t *ctx) {
    max86150_set_register(ctx, MAX86150_SYS_CONTROL, 0x1, 0x1);
}

uint8_t
max86150_get_part_id(const max86150_context_t *ctx) {
    return max86150_get_register(ctx, MAX86150_PART_ID, 0xFF);
}

// PPG Configuration

void
max86150_set_ppg_adc_range(const max86150_context_t *ctx, uint8_t range) {
    max86150_set_register(ctx, MAX86150_PPG_CONFIG1, range << 6, 0xC0);
}

void
max86150_set_ppg_sample_rate(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_PPG_CONFIG1, value << 2, 0x3C);
}

void
max86150_set_ppg_pulse_width(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_PPG_CONFIG1, value, 0x03);
}

void
max86150_set_ppg_sample_average(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_PPG_CONFIG2, value, 0x07);
}

void
max86150_set_proximity_threshold(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_PPG_PROX_INT_THRESH, value, 0xFF);
}

// LED Configuration

void
max86150_set_led_pulse_amplitude(const max86150_context_t *ctx, uint8_t led, uint8_t value) {
    uint8_t reg = MAX86150_LED1_PA;
    switch (led) {
    case 0:
        reg = MAX86150_LED1_PA;
        break;
    case 1:
        reg = MAX86150_LED2_PA;
        break;
    case 2:
        reg = MAX86150_LEDP_PA;
        break;
    default:
        break;
    }
    max86150_set_register(ctx, reg, value, 0xFF);
}

void
max86150_set_led_current_range(const max86150_context_t *ctx, uint8_t led, uint8_t value) {
    uint8_t mask = led == 0 ? 0x3 : 0xC;
    uint8_t val = led == 0 ? value : value << 2;
    max86150_set_register(ctx, MAX86150_LED_RANGE, val, mask);
}

// ECG Configuration

void
max86150_set_ecg_sample_rate(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_ECG_CONFIG1, value, 0x07);
}

void
max86150_set_ecg_pga_gain(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_ECG_CONFIG3, value << 2, 0x0C);
}

void
max86150_set_ecg_ia_gain(const max86150_context_t *ctx, uint8_t value) {
    max86150_set_register(ctx, MAX86150_ECG_CONFIG3, value, 0x03);
}

void
max86150_clear_fifo(const max86150_context_t *ctx) {
    max86150_set_fifo_wr_pointer(ctx, 0);
    max86150_set_fifo_overflow_counter(ctx, 0);
    max86150_set_fifo_rd_pointer(ctx, 0);
}
```


