#ifndef MPU6050_H_
#define MPU6050_H_ 

#include <stdint.h>
#include "i2c.h"

// MPU6050 structure
typedef struct MPU6050_t
{
 
    int16_t Accel_X_RAW;
    int16_t Accel_Y_RAW;
    int16_t Accel_Z_RAW;
    double Ax;
    double Ay;
    double Az;
 
    int16_t Gyro_X_RAW;
    int16_t Gyro_Y_RAW;
    int16_t Gyro_Z_RAW;
    double Gx;
    double Gy;
    double Gz;
 
    float Temperature;
 
    double KalmanAngleX;
    double KalmanAngleY;
} MPU6050_t;
 
// Kalman structure
typedef struct
{
    double Q_angle;
    double Q_bias;
    double R_measure;
    double angle;
    double bias;
    double P[2][2];
} Kalman_t;
 
 
double kalman_getAngle(Kalman_t *Kalman, double newAngle, double newRate, double dt);

void mpu6050_read_accel(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);
 
void mpu6050_read_gyro(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);
 
void mpu6050_read_temp(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);
  
extern void MPU6050_Read_All(I2C_HandleTypeDef *I2Cx, MPU6050_t *DataStruct);
extern uint8_t MPU6050_Init(I2C_HandleTypeDef *I2Cx);

extern MPU6050_t MPU6050;

#endif 
