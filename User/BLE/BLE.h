#ifndef BLE_H_
#define BLE_H_

#include "main.h"
#include "usart.h"
#include "Command.h"
#include <stdint.h>

#define BLE_RECV_DATA_LEN           1     //255
#define BLE_RETRY_TIMES             3
#define BLE_PACK_SIZE               20
#define BLE_BUFFERSIZE              255


typedef struct 
{
    uint8_t head;           //数据头
    int8_t ac[64];          //心率波动数据
    uint8_t heart_rate;     //心率
    uint8_t spo2;           //血氧
    uint8_t bk;             //微循环
    uint8_t fi;             //疲劳指数
    uint8_t rcv1[2];        //保留数据
    uint8_t sbp;            //收缩压
    uint8_t dbp;            //舒张压
    uint8_t rcv2[2];        //保留数据
    uint8_t sdnn;           //心率变异性
    uint8_t sport_flag;     //运动标志
} BLE_DataTypeDef;

typedef struct
{
    uint8_t data[BLE_BUFFERSIZE];
    uint8_t size;
} BLE_SendBufferTypeDef;

extern uint8_t sending_data;

extern void BLE_Init();
extern void BLE_SendData(uint8_t* data, uint16_t size);
extern void BLE_recv_callback(uint8_t*ble_recv_data);

#endif /*BLE_H_*/
