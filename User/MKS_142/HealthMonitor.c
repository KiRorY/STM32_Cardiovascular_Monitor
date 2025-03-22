#include "HealthMonitor.h"
#include "usart.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

uint8_t recv_data[1024];        //接收数据

void HealthMonitor_RT_Parser(uint8_t* data, size_t size, HealthData_RT* health_data)
{
    if (size != sizeof(HealthData_RT)){
        return;
    }
    memcpy(health_data, data, size);
}



void HealthMonitor_recv_callback(UART_HandleTypeDef *huart, size_t size)
{
    HAL_UART_Receive_IT(huart, recv_data, size);
}
