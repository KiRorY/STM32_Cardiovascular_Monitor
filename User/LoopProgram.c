#include "LoopProgram.h"
// #include "iwdg.h"
// #include "stm32f1xx_hal_iwdg.h"
#include "tim.h"
#include "BLE.h"
#include "LCD.h"
#include "HealthMonitor.h"
#include "ATK_MPU6050_SoftwareDriver/atk_ms6050.h"
#include <stdint.h>
#include <string.h>

static uint16_t ble_timer = 0;
static uint16_t loop_10ms = 0;
static uint8_t mpu6050_read_flag = 0;  // 添加MPU6050读取标志


BLE_SendBufferTypeDef ble_send_buffer;

/******************内部函数*******************/

void maintain_1ms(void)
{
    if(ble_timer > 0){
        ble_timer --;
    }
}

void maintain_10ms(void)
{
    if (ble_send_buffer.size > 0){
        //发送数据
        if (ble_send_buffer.size > BLE_PACK_SIZE){
            BLE_SendData(ble_send_buffer.data, BLE_PACK_SIZE);
            //剩余数据前推
            for (uint16_t i = 0; i < ble_send_buffer.size - BLE_PACK_SIZE; i++){
                ble_send_buffer.data[i] = ble_send_buffer.data[i + BLE_PACK_SIZE];
            }
            ble_send_buffer.size -= BLE_PACK_SIZE;
        }
        else{
            BLE_SendData(ble_send_buffer.data, ble_send_buffer.size);
            memset(&ble_send_buffer, 0, sizeof(BLE_SendBufferTypeDef));
        }
    }
    //100ms触发
    if (loop_10ms % 10 == 0){
        //设置标志位，让主循环读取MPU6050数据
        mpu6050_read_flag = 1;
    }
    loop_10ms ++;
}


/******************外部函数*******************/

uint16_t LoopProgram_BLE_Timerget(void){
    return ble_timer;
}

/**
 * @brief 设置蓝牙帧间隔时间
 */
void LoopProgram_BLE_Timerset(uint16_t timer)
{
    ble_timer = timer;
}

void LoopProgram_BLE_SendPackageset(uint8_t* data, uint16_t size)
{
    if (size > BLE_BUFFERSIZE){
        size = BLE_BUFFERSIZE;
    }
    memcpy(ble_send_buffer.data, data, size);
    ble_send_buffer.size = size;
}

/*
 * @brief 看门狗刷新
 * @note  重启时间限制为1/(40KHz/32)*2000=1600ms
 * */
void LoopProgram_WatchDog_Feed(void)
{
    // HAL_IWDG_Refresh(&hiwdg);
}

/*
 * @brief 设置看门狗线程开启和停止
 * @param status 开启:TRUE, 关闭:FALSE
 * @note 使用计数器TIM1
 * */
void LoopProgram_WatchDog_Set(BOOL status)
{
    // if(status == TRUE){
    //     HAL_TIM_Base_Start_IT(&htim1);
    // }
    // else if (status == FALSE) {
    //     HAL_TIM_Base_Stop_IT(&htim1);
    // }
}

/*
 * @brief 开启循环程序
 * @note 非看门狗
 * */
void LoopProgram_Start(void)
{
    HAL_TIM_Base_Start_IT(&htim2);
    HAL_TIM_Base_Start_IT(&htim3);

    memset(&ble_send_buffer, 0, sizeof(BLE_SendBufferTypeDef));
}

/*
 * @brief 主循环程序
 * */
void LoopProgram_Main(void)
{
    // 在主循环中处理MPU6050读取
    if (mpu6050_read_flag == 1) {
        HealthMonitor_MPU6050_AddNewData(&MPU6050_Data_history);
        mpu6050_read_flag = 0;
    }
}

/******************回调函数*******************/

//计时器回调函数重写
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    //看门狗时间:350ms一次喂狗
    if (htim->Instance == TIM1){
        LoopProgram_WatchDog_Feed();
    }

    if (htim->Instance == TIM2){
        maintain_1ms();
    }
    
    //10ms
    if (htim->Instance == TIM3){
        maintain_10ms();
    }
}

