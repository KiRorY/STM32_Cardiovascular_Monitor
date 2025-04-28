#include "HealthMonitor.h"
#include "Constant.h"
#include "LCD.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

//数据指令
const int monitor_on = 0x8A;                      //采集功能开
const int monitor_off = 0x88;                     //采集功能关
const int checkup_on = 0x8E;                      //体检功能开
const int checkup_off = 0x8C;                     //体检功能关

//显示位置
#define LCD_HR_X 90
#define LCD_HR_Y 0
#define LCD_SPO2_X LCD_HR_X
#define LCD_SPO2_Y LCD_HR_Y + 15

#define LCD_HR_LABEL_X  5
#define LCD_HR_LABEL_Y  LCD_HR_Y
#define LCD_SPO2_LABEL_X LCD_HR_LABEL_X
#define LCD_SPO2_LABEL_Y LCD_SPO2_Y

#define LCD_FONT_SIZE 12


//接收缓冲区
#define HEALTHDATA_SIZE 88
uint8_t healthdata_recv_buffer[HEALTHDATA_SIZE]; //接收数据缓存
//数据包
HealthData_RT health_data;      //实时数据

/******************内部函数*******************/


/**
 * @param : True-开启; False-关闭
 * @brief: 采集功能开关
 * */
void data_capture_op(BOOL state)
{
    if (state){
        HAL_UART_Transmit(&huart1, (uint8_t*)&monitor_on, 1, 1000);
    }
    else{
        HAL_UART_Transmit(&huart1, (uint8_t*)&monitor_off, 1, 1000);
    }
}

//解析数据
void rt_parser(uint8_t* data, size_t size, HealthData_RT* health_data)
{
    if (size != sizeof(HealthData_RT) + 1){
        return;
    }
    memcpy(health_data, data, size);                //结构体按照原始数据结构组成
}

BOOL data_is_valid(HealthData_RT* health_data)
{
    //心率波动数据全部为C4则表示手未放在传感器上
    for (int i = 0; i < sizeof(health_data->ac); i++){
        if ((health_data->ac[i] & 0xFF) != 0xC4){
            return TRUE;
        }
    }
    return FALSE;
}


/******************外部函数*******************/

/**
 * @brief: 初始化健康监测相关操作
 * */
void HealthMonitor_Init(void)
{
    LCD_ShowString(LCD_HR_LABEL_X, LCD_HR_LABEL_Y, "heart_rate:", BLACK, WHITE, LCD_FONT_SIZE,  0);
    LCD_ShowString(LCD_SPO2_LABEL_X, LCD_SPO2_LABEL_Y, "spo2:", BLACK, WHITE, LCD_FONT_SIZE,  0);
    data_capture_op(TRUE); 
}


/******************回调函数*******************/

//接收计数器
static size_t recv_counter = 0;
int pending_counter_hr = 0;
int pending_counter_spo2 = 0;
void HealthMonitor_recv_callback(uint8_t* recv_data)
{
    healthdata_recv_buffer[recv_counter] = *recv_data; //接收数据
    if (recv_counter == HEALTHDATA_SIZE - 1){
        recv_counter = 0; //接收计数器清零
    }
    else{
        recv_counter++;
        return;
    }
    
    rt_parser(healthdata_recv_buffer, HEALTHDATA_SIZE, &health_data);

    if (data_is_valid(&health_data) == FALSE){
        LCD_ShowString(LCD_HR_X, LCD_HR_Y, "          ", BLACK, WHITE, LCD_FONT_SIZE, 0);
        LCD_ShowString(LCD_SPO2_X, LCD_SPO2_Y, "          ", BLACK, WHITE, LCD_FONT_SIZE, 0);
        LCD_ShowString(LCD_HR_X, LCD_HR_Y, "Now Waiting", BLACK, WHITE, LCD_FONT_SIZE, 0);
        LCD_ShowString(LCD_SPO2_X, LCD_SPO2_Y, "Now Waiting", BLACK, WHITE, LCD_FONT_SIZE, 0);
        return;
    }

    if (health_data.heart_rate ==0){
        LCD_ShowString(LCD_HR_X, LCD_HR_Y, "           ",  BLACK, WHITE, LCD_FONT_SIZE, 0);
        switch (pending_counter_hr) {
            case 0:
                LCD_ShowString(LCD_HR_X, LCD_HR_Y, "Pending", BLACK, WHITE, LCD_FONT_SIZE, 0);
                pending_counter_hr++;
                break;
            case 1:
                LCD_ShowString(LCD_HR_X, LCD_HR_Y, "Pending.", BLACK, WHITE, LCD_FONT_SIZE, 0);
                pending_counter_hr++;
                break;
            case 2:
                LCD_ShowString(LCD_HR_X, LCD_HR_Y, "Pending..", BLACK, WHITE, LCD_FONT_SIZE, 0);
                pending_counter_hr++;
                break;
            case 3:
                LCD_ShowString(LCD_HR_X, LCD_HR_Y, "Pending...", BLACK, WHITE, LCD_FONT_SIZE, 0);
                pending_counter_hr = 0;
                break;
        }
    }
    else{
        LCD_ShowString(LCD_HR_X, LCD_HR_Y, "             ", BLACK, WHITE, LCD_FONT_SIZE, 0);
        LCD_ShowIntNum(LCD_HR_X, LCD_HR_Y, health_data.heart_rate, 3, BLACK, WHITE, LCD_FONT_SIZE);
    }

    if (health_data.spo2 == 0){
        LCD_ShowString(LCD_SPO2_X, LCD_SPO2_Y, "           ",  BLACK, WHITE, LCD_FONT_SIZE, 0);
        switch (pending_counter_spo2) {
            case 0:
                LCD_ShowString(LCD_SPO2_X, LCD_SPO2_Y, "Pending", BLACK, WHITE, LCD_FONT_SIZE, 0);
                pending_counter_spo2++;
                break;
            case 1:
                LCD_ShowString(LCD_SPO2_X, LCD_SPO2_Y, "Pending.", BLACK, WHITE, LCD_FONT_SIZE, 0);
                pending_counter_spo2++;
                break;
            case 2:
                LCD_ShowString(LCD_SPO2_X, LCD_SPO2_Y, "Pending..", BLACK, WHITE, LCD_FONT_SIZE, 0);
                pending_counter_spo2++;
                break;
            case 3:
                LCD_ShowString(LCD_SPO2_X, LCD_SPO2_Y, "Pending...", BLACK, WHITE, LCD_FONT_SIZE, 0);
                pending_counter_spo2 = 0;
                break;
        }
    }
    else {
        LCD_ShowString(LCD_SPO2_X, LCD_SPO2_Y, "             ", BLACK, WHITE, LCD_FONT_SIZE, 0);
        LCD_ShowIntNum(LCD_SPO2_X, LCD_SPO2_Y, health_data.spo2, 3, BLACK, WHITE, LCD_FONT_SIZE);
    }
    memset(healthdata_recv_buffer, 0, sizeof(healthdata_recv_buffer)); //清空接收缓存
}

