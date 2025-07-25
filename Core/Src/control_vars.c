/*
 * control_vars.c
 *
 *  Created on: Jul 22, 2025
 *      Author: hl3xs
 */


// control_vars.c

// control_vars.c
#include "control_vars.h"

ControlParams_t controlParams = {
    .current_loop = { .Kp = 600.0f, .Ki = 50.0f, .integral = 0.0f },
    .position_loop = { .Kp = 1500.0f, .Ki = 200.0f, .integral = 0.0f },
    .current_setpoint = 0.0f,
    .position_setpoint = 2500.0f,
    .position_scale = 1000.0f
};
