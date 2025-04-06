#include "LoopProgram.h"
#include "iwdg.h"
#include "stm32f1xx_hal_iwdg.h"
#include "tim.h"
#include "LCD.h"
#include "MPU6050.h"
#include <stdint.h>

static uint16_t ble_timer = 0;
static uint16_t loop_10ms = 0;

MPU6050_t MPU6050;

/******************内部函数*******************/

void maintain_1ms(void)
{
    if(ble_timer){
        ble_timer --;
    }
}

void maintain_10ms(void)
{
    //100ms触发
    if (loop_10ms % 10 == 0){
    }
}


/******************外部函数*******************/


/**
 * @brief 设置蓝牙帧间隔时间
 */
uint16_t LoopProgram_BLE_Timerset(void)
{
    return ble_timer; 
}

/*
 * @brief 看门狗刷新
 * @note  重启时间限制为1/(40KHz/32)*2000=1600ms
 * */
void LoopProgram_WatchDog_Feed(void)
{
    HAL_IWDG_Refresh(&hiwdg);
}

/*
 * @brief 设置看门狗线程开启和停止
 * @param status 开启:TRUE, 关闭:FALSE
 * @note 使用计数器TIM1
 * */
void LoopProgram_WatchDog_Set(BOOL status)
{
    if(status == TRUE){
        HAL_TIM_Base_Start_IT(&htim1);
    }
    else if (status == FALSE) {
        HAL_TIM_Base_Stop_IT(&htim1);
    }
}

/*
 * @brief 开启循环程序
 * @note 非看门狗
 * */
void LoopProgram_Start(void)
{
    HAL_TIM_Base_Start_IT(&htim3);
}

/*
 * @brief 主循环程序
 * */
void LoopProgram_Main(void)
{

}

/******************回调函数*******************/

//计时器回调函数重写
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    //看门狗时间:350ms一次喂狗
    if (htim->Instance == TIM1){
        LoopProgram_WatchDog_Feed();
    }
    
    //10ms
    if (htim->Instance == TIM3){
        maintain_10ms();
    }
}

