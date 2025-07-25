#include "main.h"
#include "control_vars.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "uart_command.h"

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern ControlParams_t controlParams;

#define UART_RX_BUF_SIZE 128
uint8_t uart_rx_buf[UART_RX_BUF_SIZE];

void UartReceiveTask(void *argument) {
    char rx_buf[64];
    uint16_t idx = 0;
    char ch;

    for (;;) {
        if (HAL_UART_Receive(&huart2, (uint8_t *)&ch, 1, HAL_MAX_DELAY) == HAL_OK) {
            if (ch == '\n') {
                rx_buf[idx] = '\0';
                ProcessUartCommand(rx_buf);
                idx = 0;
            } else if (idx < sizeof(rx_buf) - 1) {
                rx_buf[idx++] = ch;
            } else {
                idx = 0;  // buffer overflow → discard
            }
        }
    }
}

void ProcessUartCommand(char *cmd) {
    int p_kp_i, p_ki_i, c_kp_i, c_ki_i;
    int p_i_i, c_i_i, cset_i, pset_i, psca_i;

    if (strncmp(cmd, "P_KP=", 5) == 0) {
        int parsed = sscanf(cmd, "P_KP=%d,P_KI=%d,C_KP=%d,C_KI=%d",
                            &p_kp_i, &p_ki_i, &c_kp_i, &c_ki_i);
        if (parsed == 4) {
            controlParams.position_loop.Kp = p_kp_i;
            controlParams.position_loop.Ki = p_ki_i;
            controlParams.current_loop.Kp  = c_kp_i;
            controlParams.current_loop.Ki  = c_ki_i;
        }
    }

    else if (strncmp(cmd, "ALL=", 4) == 0) {
        int parsed = sscanf(cmd + 4,
            "P_KP=%d,P_KI=%d,P_I=%d,C_KP=%d,C_KI=%d,C_I=%d,CSET=%d,PSET=%d,PSCA=%d",
            &p_kp_i, &p_ki_i, &p_i_i, &c_kp_i, &c_ki_i, &c_i_i, &cset_i, &pset_i, &psca_i);

        if (parsed == 9) {
            controlParams.position_loop.Kp        = p_kp_i;
            controlParams.position_loop.Ki        = p_ki_i;
            controlParams.position_loop.integral  = p_i_i;

            controlParams.current_loop.Kp         = c_kp_i;
            controlParams.current_loop.Ki         = c_ki_i;
            controlParams.current_loop.integral   = c_i_i;

            controlParams.current_setpoint        = cset_i;
            controlParams.position_setpoint       = pset_i;
            controlParams.position_scale          = psca_i;
        }
    }
}

void ParseUARTCommand(char *cmd) {
    int val = 0;

    if (strncmp(cmd, "Px", 2) == 0) {
        val = atoi(&cmd[2]);
        controlParams.current_loop.Kp = val;
    } else if (strncmp(cmd, "Ix", 2) == 0) {
        val = atoi(&cmd[2]);
        controlParams.current_loop.Ki = val;
    } else if (strncmp(cmd, "Dx", 2) == 0) {
        val = atoi(&cmd[2]);
        controlParams.current_loop.integral = val;
    } else if (strncmp(cmd, "Py", 2) == 0) {
        val = atoi(&cmd[2]);
        controlParams.position_loop.Kp = val;
    } else if (strncmp(cmd, "Iy", 2) == 0) {
        val = atoi(&cmd[2]);
        controlParams.position_loop.Ki = val;
    } else if (strncmp(cmd, "Dy", 2) == 0) {
        val = atoi(&cmd[2]);
        controlParams.position_loop.integral = val;
    } else if (strncmp(cmd, "CSET", 4) == 0) {
        val = atoi(&cmd[4]);
        controlParams.current_setpoint = val;
    } else if (strncmp(cmd, "PSET", 4) == 0) {
        val = atoi(&cmd[4]);
        controlParams.position_setpoint = val;
    } else if (strncmp(cmd, "PSCA", 4) == 0) {
        val = atoi(&cmd[4]);
        controlParams.position_scale = val;
    } else if (strncmp(cmd, "ST", 2) == 0) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer),
                 "Kp1=%d Ki1=%d I1=%d\r\n"
                 "Kp2=%d Ki2=%d I2=%d\r\n"
                 "CSET=%d PSET=%d PSCA=%d\r\n",
                 controlParams.current_loop.Kp,
                 controlParams.current_loop.Ki,
                 controlParams.current_loop.integral,
                 controlParams.position_loop.Kp,
                 controlParams.position_loop.Ki,
                 controlParams.position_loop.integral,
                 controlParams.current_setpoint,
                 controlParams.position_setpoint,
                 controlParams.position_scale);
        HAL_UART_Transmit(&huart1, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
    } else if (strncmp(cmd, "H", 1) == 0) {
        const char *help_msg =
            "Pxnn: Kp1, Ixnn: Ki1, Dxnn: I1\r\n"
            "Pynn: Kp2, Iynn: Ki2, Dynn: I2\r\n"
            "CSETnnn: current_setpoint (*100)\r\n"
            "PSETnnn: position_setpoint (*100)\r\n"
            "PSCAnnn: position_scale (*1000)\r\n"
            "ALL=...: all parameters\r\n"
            "ST: show all variables\r\n";
        HAL_UART_Transmit(&huart1, (uint8_t*)help_msg, strlen(help_msg), HAL_MAX_DELAY);
    }
}
