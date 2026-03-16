#ifndef MPU6050_H
#define MPU6050_H

#include "stm32f4xx_hal.h"   // change to your exact MCU family
#include <stdint.h>
#include <stdbool.h>

/* ---------- DEVICE ADDRESS ---------- */

#define MPU6050_ADDR                  0x68

/* ---------- REGISTERS ---------- */

#define MPU6050_SMPLRT_DIV_REGISTER   0x19
#define MPU6050_CONFIG_REGISTER       0x1A
#define MPU6050_GYRO_CONFIG_REGISTER  0x1B
#define MPU6050_ACCEL_CONFIG_REGISTER 0x1C
#define MPU6050_PWR_MGMT_1_REGISTER   0x6B

#define MPU6050_ACCEL_OUT_REGISTER    0x3B
#define MPU6050_GYRO_OUT_REGISTER     0x43

/* ---------- CONSTANTS ---------- */

#define RAD_2_DEG             57.29578f
#define CALIB_OFFSET_NB_MES   500
#define TEMP_LSB_2_DEGREE     340.0f
#define TEMP_LSB_OFFSET       12412.0f

#define DEFAULT_GYRO_COEFF    0.98f

/* ---------- MPU STRUCT ---------- */

typedef struct
{
    /* HAL */
    I2C_HandleTypeDef *hi2c;

    /* Address */
    uint8_t address;

    /* Scaling */
    float gyro_lsb_to_degsec;
    float acc_lsb_to_g;

    /* Offsets */
    float gyroXoffset;
    float gyroYoffset;
    float gyroZoffset;

    float accXoffset;
    float accYoffset;
    float accZoffset;

    /* Raw / scaled data */
    float temp;
    float accX;
    float accY;
    float accZ;
    float gyroX;
    float gyroY;
    float gyroZ;

    /* Angles */
    float angleAccX;
    float angleAccY;

    float angleX;
    float angleY;
    float angleZ;

    /* Filter */
    float filterGyroCoef;

    /* Timing */
    uint32_t preInterval;
    uint32_t prevTick;

    /* Mounting */
    bool upsideDownMounting;

} MPU6050_t;

/* ---------- API ---------- */

/* Init */
uint8_t MPU6050_Init(MPU6050_t *mpu,
                     I2C_HandleTypeDef *hi2c,
                     uint8_t gyro_cfg,
                     uint8_t acc_cfg);

/* Configuration */
uint8_t MPU6050_SetGyroConfig(MPU6050_t *mpu, uint8_t cfg);
uint8_t MPU6050_SetAccConfig(MPU6050_t *mpu, uint8_t cfg);

void MPU6050_SetGyroOffsets(MPU6050_t *mpu, float x, float y, float z);
void MPU6050_SetAccOffsets (MPU6050_t *mpu, float x, float y, float z);

void MPU6050_SetFilterGyroCoef(MPU6050_t *mpu, float coef);
static inline void MPU6050_SetFilterAccCoef(MPU6050_t *mpu, float coef)
{
    MPU6050_SetFilterGyroCoef(mpu, 1.0f - coef);
}

/* Data */
void MPU6050_FetchData(MPU6050_t *mpu);
void MPU6050_Update(MPU6050_t *mpu);
void MPU6050_CalcOffsets(MPU6050_t *mpu, uint8_t calcGyro, uint8_t calcAcc);
#endif /* MPU6050_H */
