#include "BLE.h"
#include "Command.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include "LCD.h"
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
    HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t*) ble_recv_data, sizeof(ble_recv_data));
}

/******************回调函数*******************/

void BLE_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
    // HAL_UARTEx_ReceiveToIdle_IT(&huart1, (uint8_t*)ble_recv_data, size);
    HAL_UART_Receive_IT((huart), (uint8_t*)ble_recv_char, 1);
    Command_Typedef cmd;
    if (Command_Parse((uint8_t*)ble_recv_data, size, &cmd) == TRUE){
        //解析成功
        LCD_ShowString(20, 20, cmd.data, BLACK, BLACK, 12, 0);
    }
}
