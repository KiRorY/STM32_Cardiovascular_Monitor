#ifndef LOOPPROGRAM_H_
#define LOOPPROGRAM_H_

#include "Constant.h"
#include <stdint.h>

extern uint16_t LoopProgram_BLE_Timerget(void);
extern void LoopProgram_BLE_Timerset(uint16_t timer);
extern void LoopProgram_BLE_SendPackageset(uint8_t* data, uint16_t size);
extern void LoopProgram_WatchDog_Feed(void);
extern void LoopProgram_WatchDog_Set(BOOL status);
extern void LoopProgram_Start(void);
extern void LoopProgram_Main(void);
extern void LoopProgram_MPU6050_Process(void);
#endif /*LOOPPROGRAM_H_*/
