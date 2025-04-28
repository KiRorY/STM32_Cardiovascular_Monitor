#include "BLE.h"
#include "Command.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include <stdint.h>
#include <string.h>

static char ble_recv_data[BLE_RECV_DATA_LEN];
static char ble_recv_char;


/******************外部函数*******************/

/* 
 * @brief 蓝牙初始化
 * */
void BLE_Init()
{
    memset(ble_recv_data, 0, sizeof(ble_recv_data)); 
}

void BLE_SendData(uint8_t* data, uint16_t size)
{
    HAL_UART_Transmit(&huart2, data, size, DATALEN);
}

/******************回调函数*******************/

void BLE_recv_callback(uint8_t*ble_recv_data)
{
    BLECommandTypedef cmd;
}
