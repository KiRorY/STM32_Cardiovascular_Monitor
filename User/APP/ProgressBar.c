#include "ProgressBar.h"
#include "Constant.h"
#include "LCD.h"
#include "LCD_Init.h"
#include <stdint.h>

#define MAX_PROGRESS 100
#define MIN_PROGRESS 0
#define BAR_XSTART   0                      //设定进度条x轴起始位置
#define BAR_XEND     LCD_W                  //设定进度条x轴最大终止位置
#define BAR_YSTART   LCD_H - 10             //设定进度条y轴位置
#define BAR_YEND     LCD_H


/******************外部函数*******************/

/*
 * @brief 清除进度条
 * */
void ProgressBar_Clear(void)
{
    LCD_Fill(0, LCD_H - 10, LCD_W, LCD_H, WHITE);
}


/*
 * @brief 设置进度条进度与颜色
 * */
void ProgressBar_SetProgress(uint16_t rate, uint16_t color)
{
    uint16_t actual_rate;
    //防止溢出
    if (rate > MAX_PROGRESS){
        actual_rate = MAX_PROGRESS;
    }
    else if (rate < MIN_PROGRESS){
        actual_rate = MIN_PROGRESS; 
    }
    else {
        actual_rate = rate;
    }
    
    //进度与屏幕实际大小的比率  1:ratio
    float int_ratio = (float)(BAR_XEND - BAR_XSTART) / (float)(MAX_PROGRESS - MIN_PROGRESS);

    //通过四舍五入取到整数长度
    uint16_t bar_length = ROUND_TO_UINT16(int_ratio * rate);

    if (actual_rate != MAX_PROGRESS){
        LCD_Fill(BAR_XSTART, BAR_YSTART, bar_length + BAR_XSTART, BAR_YEND, color);
    }
    else if (actual_rate == MAX_PROGRESS) {
        LCD_Fill(BAR_XSTART, BAR_YSTART, BAR_XEND, BAR_YEND, color);
    }
}
