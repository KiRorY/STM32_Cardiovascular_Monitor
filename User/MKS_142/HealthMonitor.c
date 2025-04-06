#include "HealthMonitor.h"
#include "Constant.h"
#include "LCD.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

const int monitor_on = 0x8A;                      //采集功能开
const int monitor_off = 0x88;                     //采集功能关
const int checkup_on = 0x8E;                      //体检功能开
const int checkup_off = 0x8C;                     //体检功能关
//
HealthData_RT health_data;      //实时数据

//数据采集状态更改
//@param state: True-开启; False-关闭
void data_capture_op(BOOL state)
{
    if (state){
        HAL_UART_Transmit(&huart1, (uint8_t*)&monitor_on, 1, 1000);
    }
    else{
        HAL_UART_Transmit(&huart1, (uint8_t*)&monitor_off, 1, 1000);
    }
}

void rt_parser(uint8_t* data, size_t size, HealthData_RT* health_data)
{
    if (size != sizeof(HealthData_RT)){
        return;
    }
    memcpy(health_data, data, size);                //结构体按照原始数据结构组成
}

void HealthMonitor_Init(void)
{
    data_capture_op(TRUE);
}

static int recv_counter = 0;
void HealthMonitor_recv_callback(uint8_t* recv_data, size_t size)
{
    rt_parser(recv_data, size, &health_data);
    LCD_ShowIntNum(5, 16, health_data.heart_rate, 5, BLACK, WHITE, 16); //心率
    recv_counter++;
}

