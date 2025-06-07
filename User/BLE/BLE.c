#include "BLE.h"
#include "Command.h"
#include "LoopProgram.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include <stdint.h>
#include <string.h>

static char ble_recv_data[BLE_RECV_DATA_LEN];

uint8_t sending_data = 0;

/******************外部函数*******************/

/* 
 * @brief 蓝牙初始化
 * */
void BLE_Init()
{
    memset(ble_recv_data, 0, sizeof(ble_recv_data)); 
    sending_data = 0;
}

void BLE_SendData(uint8_t* data, uint16_t size)
{
    HAL_UART_Transmit(&huart2, data, size, 1000); //发送数据
}

/******************回调函数*******************/

static int recv_counter = 0;
void BLE_recv_callback(uint8_t*ble_recv_data)
{
    ble_recv_data[recv_counter] = *ble_recv_data; //接收数据
    recv_counter ++;
    if (recv_counter == BLE_RECV_DATA_LEN){
        recv_counter = 0; //接收计数器清零
        if (ble_recv_data[0]== 0xAA){
            sending_data = 1;
        }
        else if (ble_recv_data[0] == 0XAB){
            sending_data = 0;
        }
        else{
            memset(ble_recv_data, 0, BLE_RECV_DATA_LEN); //清空接收数据
        }
    }
    else{
        return;
    }
}
