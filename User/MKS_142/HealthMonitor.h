#ifndef HEALTHMONITOR_H_
#define HEALTHMONITOR_H_
#include <stdint.h>
#include <stddef.h>
#include "Constant.h"

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
    uint8_t rmssd;
    uint8_t nn50;
    uint8_t pnn50;
    uint8_t rra[6];
    uint8_t trsv2[2];
} HealthData_RT;

typedef struct
{
    uint16_t head;           //体检数据包头
    uint16_t sequence;       //体检数据包序号
    uint8_t id[16];          //唯一硬件编号
    uint8_t tst30_data[168]; //体检数据
    uint8_t rsv1[6];         //保留数据
    uint16_t CRC16;
} TST30_PACK_S;

void data_capture_op(BOOL state);
void rt_parser(uint8_t* data, size_t size, HealthData_RT* health_data);

extern void HealthMonitor_Init(void);
extern void HealthMonitor_recv_callback(uint8_t* recv_data);

#endif /*HEALTHMONITOR_H_*/
