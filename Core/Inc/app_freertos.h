/*
 * app_freertos.h
 *
 *  Created on: Jul 23, 2025
 *      Author: hl3xs
 */

// app_freertos.h
#ifndef __APP_FREERTOS_H__
#define __APP_FREERTOS_H__

#ifdef __cplusplus
extern "C" {
#endif

void MX_FREERTOS_Init(void);     // FreeRTOS 기본 초기화
void StartControlTasks(void);    // 사용자 정의 Task 생성

#ifdef __cplusplus
}
#endif

#endif /* __APP_FREERTOS_H__ */
