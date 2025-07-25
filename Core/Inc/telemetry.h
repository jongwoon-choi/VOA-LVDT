/*
 * telemetry.h
 *
 *  Created on: Jul 23, 2025
 *      Author: hl3xs
 */

#ifndef TELEMETRY_H
#define TELEMETRY_H

#include <stdint.h>
#include "cmsis_os.h"

typedef struct {
    uint8_t  header;
    uint16_t pos_set_dac;
    int16_t  pos_fb_adc;
    uint16_t cur_set_dac;
    uint16_t cur_fb_adc;
    int16_t  pos_err_dac;
    int16_t  cur_err_adc;
    uint16_t ctrl_dac_val;
    int16_t  pos_out_raw;
    int16_t  cur_out_raw;
    uint8_t  crc8;
    uint8_t  footer;
} TelemetryPacket_t;

void InitTelemetrySystem(void);  // 초기화
void EnqueueTelemetryPacket(uint16_t pos_set_dac,
                            int16_t  pos_fb_adc,
                            uint16_t cur_set_dac,
                            uint16_t cur_fb_adc,
                            uint16_t ctrl_dac_val,
                            int16_t  pos_out_raw,
                            int16_t  cur_out_raw);

#endif  // TELEMETRY_H
