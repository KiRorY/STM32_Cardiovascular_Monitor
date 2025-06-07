#ifndef HEALTHMONITOR_H_
#define HEALTHMONITOR_H_
#include <stdint.h>
#include <stddef.h>
#include "Constant.h"

#define MPU_HISTORY_SIZE 12

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
    float Ax[MPU_HISTORY_SIZE];              //加速度，单位为g,范围在+-2g，因此需要除以16384
    float Ay[MPU_HISTORY_SIZE];
    float Az[MPU_HISTORY_SIZE];
 
    float Gx[MPU_HISTORY_SIZE];              //角度陀螺仪，单位为度/S，范围+-2000，除以16.4
    float Gy[MPU_HISTORY_SIZE];
    float Gz[MPU_HISTORY_SIZE];
 
    int16_t Temperature[MPU_HISTORY_SIZE];    //除以精确倍数100
    uint8_t current_index;
 
} MPU6050History_t;

// typedef struct
// {
//     uint16_t head;           //体检数据包头
//     uint16_t sequence;       //体检数据包序号
//     uint8_t id[16];          //唯一硬件编号
//     uint8_t tst30_data[168]; //体检数据
//     uint8_t rsv1[6];         //保留数据
//     uint16_t CRC16;
// } TST30_PACK_S;

extern MPU6050History_t MPU6050_Data_history;

void data_capture_op(BOOL state);
void rt_parser(uint8_t* data, size_t size, HealthData_RT* health_data);

extern void HealthMonitor_Init(void);
extern void HealthMonitor_MPU6050_AddNewData(MPU6050History_t* mpu6050_data_history);
extern void HealthMonitor_recv_callback(uint8_t* recv_data);

#endif /*HEALTHMONITOR_H_*/
