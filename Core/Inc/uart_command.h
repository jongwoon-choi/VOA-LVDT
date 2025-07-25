/*
 * uart_command.h
 *
 *  Created on: Jul 23, 2025
 *      Author: hl3xs
 */

#ifndef UART_COMMAND_H
#define UART_COMMAND_H

void ProcessUartCommand(char *cmd);
void UartReceiveTask(void *argument);

#endif
