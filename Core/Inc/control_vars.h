#ifndef __CONTROL_VARS_H__
#define __CONTROL_VARS_H__

#include <stdint.h>  // <<< 추가!

typedef struct {
    int16_t Kp;        // 원래 값 * 100
    int16_t Ki;        // 원래 값 * 100
    int16_t integral;  // 원래 값 * 100
} LoopParams_t;

typedef struct {
    LoopParams_t current_loop;
    LoopParams_t position_loop;
    int16_t current_setpoint;     // *100
    int16_t position_setpoint;    // *100
    int16_t position_scale;       // *1000
} ControlParams_t;


extern ControlParams_t controlParams;

#endif
