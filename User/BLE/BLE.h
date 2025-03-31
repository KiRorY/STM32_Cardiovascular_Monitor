#ifndef BLE_H_
#define BLE_H_

#include "main.h"
#include "usart.h"
#include "Command.h"

#define BLE_RECV_DATA_LEN           DATALEN     //255
#define BLE_RETRY_TIMES             3


extern void BLE_Init();
extern void BLE_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size);

#endif /*BLE_H_*/
