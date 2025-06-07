#include "HealthMonitor.h"
#include "BLE.h"
#include "atk_ms6050.h"
#include "Constant.h"
#include "LCD.h"
#include "stm32f1xx_hal_uart.h"
#include "usart.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

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
MPU6050History_t MPU6050_Data_history; //运动历史数据

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

uint8_t get_sport_flag(MPU6050History_t* mpu6050_data_history, uint8_t heart_rate)
{
    if (mpu6050_data_history->current_index == 0){
        return 0;
    }
    
    float acc_var_sum = 0;  // 加速度变化量累计
    float gyro_sum = 0;     // 角速度幅值累计
    int motion_points = 0;  // 运动点计数
    
    int data_count = (mpu6050_data_history->current_index < MPU_HISTORY_SIZE) ? mpu6050_data_history->current_index : MPU_HISTORY_SIZE;
    
    // 分析历史数据点
    for (int i = 0; i < data_count; i++) {
        // 计算总加速度幅值
        float total_acc = sqrtf(mpu6050_data_history->Ax[i] * mpu6050_data_history->Ax[i] + 
                               mpu6050_data_history->Ay[i] * mpu6050_data_history->Ay[i] + 
                               mpu6050_data_history->Az[i] * mpu6050_data_history->Az[i]);
        
        // 正确剔除重力：动态加速度 = 总加速度 - 重力加速度(1g)
        float dynamic_acc = fabsf(total_acc - 1.0f);
        
        // 计算角速度幅值
        float gyro_mag = sqrtf(mpu6050_data_history->Gx[i] * mpu6050_data_history->Gx[i] + 
                              mpu6050_data_history->Gy[i] * mpu6050_data_history->Gy[i] + 
                              mpu6050_data_history->Gz[i] * mpu6050_data_history->Gz[i]);
        
        acc_var_sum += dynamic_acc;
        gyro_sum += gyro_mag;
        
        // 计算运动点数
        if (dynamic_acc > 0.2f || gyro_mag > 30.0f) {
            motion_points++;
        }
    }
    
    // 计算平均值
    float avg_dynamic_acc = acc_var_sum / data_count;
    float avg_gyro = gyro_sum / data_count;
    
    // 运动判断逻辑
    int sports_flag = 0;

    // 综合判断：要求同时满足加速度和角速度条件，并且运动点数要足够多
    if (avg_dynamic_acc > 0.3f &&    // 平均动态加速度超过阈值
        avg_gyro > 45.0f &&          // 平均角速度超过阈值
        motion_points >= data_count / 2) {  // 运动点数超过一半
        sports_flag = 1;
    }

    // 心率作为独立的辅助判断条件
    if (heart_rate > 110 && (avg_dynamic_acc > 0.2f || avg_gyro > 25.0f)) {
        sports_flag = 1;
    }
    
    return sports_flag;
}

/******************外部函数*******************/

/**
 * @brief: 初始化健康监测相关操作
 * */
void HealthMonitor_Init(void)
{
    LCD_ShowString(LCD_HR_LABEL_X, LCD_HR_LABEL_Y, "heart_rate:", BLACK, WHITE, LCD_FONT_SIZE,  0);
    LCD_ShowString(LCD_SPO2_LABEL_X, LCD_SPO2_LABEL_Y, "spo2:", BLACK, WHITE, LCD_FONT_SIZE,  0);
    memset(MPU6050_Data_history.Ax, 0, sizeof(MPU6050_Data_history.Ax));
    memset(MPU6050_Data_history.Ay, 0, sizeof(MPU6050_Data_history.Ay));
    memset(MPU6050_Data_history.Az, 0, sizeof(MPU6050_Data_history.Az));
    memset(MPU6050_Data_history.Gx, 0, sizeof(MPU6050_Data_history.Gx));
    memset(MPU6050_Data_history.Gy, 0, sizeof(MPU6050_Data_history.Gy));
    memset(MPU6050_Data_history.Gz, 0, sizeof(MPU6050_Data_history.Gz));
    memset(MPU6050_Data_history.Temperature, 0, sizeof(MPU6050_Data_history.Temperature));
    MPU6050_Data_history.current_index = 0;

    data_capture_op(TRUE); 
}

void HealthMonitor_MPU6050_AddNewData(MPU6050History_t* mpu6050_data_history)
{
    int16_t ax, ay, az;
    int16_t gx, gy, gz;
    int16_t temp;

    int8_t ret = atk_ms6050_get_accelerometer(&ax, &ay, &az);
    ret += atk_ms6050_get_gyroscope(&gx, &gy, &gz);
    ret += atk_ms6050_get_temperature(&temp);

    if (ret == 0){
        if (mpu6050_data_history->current_index < MPU_HISTORY_SIZE - 1){
            mpu6050_data_history->Ax[mpu6050_data_history->current_index] = (float)ax/16384;
            mpu6050_data_history->Ay[mpu6050_data_history->current_index] = (float)ay/16384;
            mpu6050_data_history->Az[mpu6050_data_history->current_index] = (float)az/16384;
            mpu6050_data_history->Gx[mpu6050_data_history->current_index] = (float)gx/16.4;
            mpu6050_data_history->Gy[mpu6050_data_history->current_index] = (float)gy/16.4;
            mpu6050_data_history->Gz[mpu6050_data_history->current_index] = (float)gz/16.4;
            mpu6050_data_history->Temperature[mpu6050_data_history->current_index] = temp/100;
            mpu6050_data_history->current_index++;
        }
        else if (mpu6050_data_history->current_index == MPU_HISTORY_SIZE - 1){
            for (int i = 0; i < MPU_HISTORY_SIZE - 1; i++){
                mpu6050_data_history->Ax[i] = mpu6050_data_history->Ax[i + 1];
                mpu6050_data_history->Ay[i] = mpu6050_data_history->Ay[i + 1];
                mpu6050_data_history->Az[i] = mpu6050_data_history->Az[i + 1];
                mpu6050_data_history->Gx[i] = mpu6050_data_history->Gx[i + 1];
                mpu6050_data_history->Gy[i] = mpu6050_data_history->Gy[i + 1];
                mpu6050_data_history->Gz[i] = mpu6050_data_history->Gz[i + 1];
                mpu6050_data_history->Temperature[i] = mpu6050_data_history->Temperature[i + 1];
            }
            mpu6050_data_history->Ax[MPU_HISTORY_SIZE - 1] = (float)ax / 16384;
            mpu6050_data_history->Ay[MPU_HISTORY_SIZE - 1] = (float)ay / 16384;
            mpu6050_data_history->Az[MPU_HISTORY_SIZE - 1] = (float)az / 16384;
            mpu6050_data_history->Gx[MPU_HISTORY_SIZE - 1] = (float)gx / 16.4;
            mpu6050_data_history->Gy[MPU_HISTORY_SIZE - 1] = (float)gy / 16.4;
            mpu6050_data_history->Gz[MPU_HISTORY_SIZE - 1] = (float)gz / 16.4;
            mpu6050_data_history->Temperature[MPU_HISTORY_SIZE - 1] = temp / 100;
        }
    }
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

    //解析
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

    if (sending_data == 1){
        BLE_DataTypeDef ble_data;
        memcpy(&ble_data, &health_data, sizeof(ble_data) - sizeof(ble_data.sport_flag));
        ble_data.sport_flag = get_sport_flag(&MPU6050_Data_history, health_data.heart_rate);
        BLE_SendData((uint8_t*)&ble_data, sizeof(ble_data));
        sending_data = 0;
    }
    memset(healthdata_recv_buffer, 0, sizeof(healthdata_recv_buffer)); //清空接收缓存    
}

