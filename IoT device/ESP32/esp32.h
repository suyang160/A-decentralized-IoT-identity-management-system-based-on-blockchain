#ifndef _ESP32_H_
#define _ESP32_H_

/*The program in this file and esp32.h has referred to the project of 正点原子 about ESP8266 used in stm32F149
 * Thanks for their contribution
 * which can be found in http://www.openedv.com/
 * */
#include "FreeRTOS.h"
#include "task.h"
#include "fsl_usart_freertos.h"
#include "fsl_usart.h"
#include "fsl_debug_console.h"
#include "string.h"

#define WiFi_MAX_RECV_LEN		2000					//最大接收缓存字节数

extern usart_rtos_handle_t wifi_handle;

extern uint16_t WiFi_RX_STA;
extern uint8_t WiFi_RX_BUF[WiFi_MAX_RECV_LEN];
/*
 * not connect to wifi 0;
 * connect to wifi 1;
 * connect to TCP  2;
 * */
extern uint8_t current_network_state;

/*
 * value is 1, end with '\r\n'
 * value is 2, end with '}'
 * */
extern uint8_t data_end_flag;

/*
 * value is 0, start receive wifi data
 * value is 1, stop receive wifi data
 * */
extern uint8_t start_receive_data_flag;

uint8_t* esp32_check_cmd(uint8_t *str);
uint8_t esp32_send_cmd(uint8_t *cmd,uint8_t *ack,uint32_t waittime);
uint8_t esp32_send_data(uint8_t *data,uint8_t *ack,uint32_t waittime);

#endif
