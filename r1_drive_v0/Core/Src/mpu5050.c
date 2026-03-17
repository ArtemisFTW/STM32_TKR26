#include "mpu6050.h"
#include <math.h>

/* ---------- INTERNAL HELPER ---------- */

/* Wrap angle to [-limit, +limit] */
static float wrap(float angle, float limit)
{
	while (angle >  limit) angle -= 2.0f * limit;
	while (angle < -limit) angle += 2.0f * limit;
	return angle;
}

static float apply_deadband(float value, float deadband)
{
    if (fabsf(value) < deadband)
        return 0.0f;
    return value;
}

/* ---------- LOW LEVEL I2C ---------- */

static HAL_StatusTypeDef MPU6050_WriteByte(MPU6050_t *mpu,
		uint8_t reg,
		uint8_t data)
{
	return HAL_I2C_Mem_Write(
			mpu->hi2c,
			MPU6050_ADDR << 1,
			reg,
			I2C_MEMADD_SIZE_8BIT,
			&data,
			1,
			HAL_MAX_DELAY
	);
}
static HAL_StatusTypeDef MPU6050_ReadBytes(MPU6050_t *mpu,
		uint8_t reg,
		uint8_t *buf,
		uint16_t len)
{
	return HAL_I2C_Mem_Read(
			mpu->hi2c,
			MPU6050_ADDR << 1,
			reg,
			I2C_MEMADD_SIZE_8BIT,
			buf,
			len,
			HAL_MAX_DELAY
	);
}

/* ---------- INIT ---------- */

uint8_t MPU6050_Init(MPU6050_t *mpu,
		I2C_HandleTypeDef *hi2c,
		uint8_t gyro_cfg,
		uint8_t acc_cfg)
{
	mpu->hi2c = hi2c;

	/* Wake up device */
	if (MPU6050_WriteByte(mpu, MPU6050_PWR_MGMT_1_REGISTER, 0x01) != HAL_OK)
		return 1;

	MPU6050_WriteByte(mpu, MPU6050_SMPLRT_DIV_REGISTER, 0x00);
	MPU6050_WriteByte(mpu, MPU6050_CONFIG_REGISTER, 0x00);

	MPU6050_SetGyroConfig(mpu, gyro_cfg);
	MPU6050_SetAccConfig(mpu, acc_cfg);

	/* Defaults */
	MPU6050_SetGyroOffsets(mpu, 0, 0, 0);
	MPU6050_SetAccOffsets(mpu, 0, 0, 0);
	MPU6050_SetFilterGyroCoef(mpu, DEFAULT_GYRO_COEFF);

	/* First update */
	MPU6050_Update(mpu);
	mpu->angleX = mpu->angleAccX;
	mpu->angleY = mpu->angleAccY;
	mpu->angleZ = 0.0f;

	mpu->prevTick = HAL_GetTick();
	mpu->preInterval = HAL_GetTick();
	return 0;
}

/* ---------- CONFIG ---------- */

uint8_t MPU6050_SetGyroConfig(MPU6050_t *mpu, uint8_t cfg)
{
	switch (cfg)
	{
	case 0: mpu->gyro_lsb_to_degsec = 131.0f;
	return MPU6050_WriteByte(mpu, MPU6050_GYRO_CONFIG_REGISTER, 0x00);
	case 1: mpu->gyro_lsb_to_degsec = 65.5f;
	return MPU6050_WriteByte(mpu, MPU6050_GYRO_CONFIG_REGISTER, 0x08);
	case 2: mpu->gyro_lsb_to_degsec = 32.8f;
	return MPU6050_WriteByte(mpu, MPU6050_GYRO_CONFIG_REGISTER, 0x10);
	case 3: mpu->gyro_lsb_to_degsec = 16.4f;
	return MPU6050_WriteByte(mpu, MPU6050_GYRO_CONFIG_REGISTER, 0x18);
	default: return 1;
	}
}

uint8_t MPU6050_SetAccConfig(MPU6050_t *mpu, uint8_t cfg)
{
	switch (cfg)
	{
	case 0: mpu->acc_lsb_to_g = 16384.0f;
	return MPU6050_WriteByte(mpu, MPU6050_ACCEL_CONFIG_REGISTER, 0x00);
	case 1: mpu->acc_lsb_to_g = 8192.0f;
	return MPU6050_WriteByte(mpu, MPU6050_ACCEL_CONFIG_REGISTER, 0x08);
	case 2: mpu->acc_lsb_to_g = 4096.0f;
	return MPU6050_WriteByte(mpu, MPU6050_ACCEL_CONFIG_REGISTER, 0x10);
	case 3: mpu->acc_lsb_to_g = 2048.0f;
	return MPU6050_WriteByte(mpu, MPU6050_ACCEL_CONFIG_REGISTER, 0x18);
	default: return 1;
	}
}

/* ---------- OFFSETS ---------- */

void MPU6050_SetGyroOffsets(MPU6050_t *mpu, float x, float y, float z)
{
	mpu->gyroXoffset = x;
	mpu->gyroYoffset = y;
	mpu->gyroZoffset = z;
}

void MPU6050_SetAccOffsets(MPU6050_t *mpu, float x, float y, float z)
{
	mpu->accXoffset = x;
	mpu->accYoffset = y;
	mpu->accZoffset = z;
}

void MPU6050_SetFilterGyroCoef(MPU6050_t *mpu, float coef)
{
	if (coef < 0.0f || coef > 1.0f)
		coef = DEFAULT_GYRO_COEFF;

	mpu->filterGyroCoef = coef;
}

/* ---------- DATA ---------- */

void MPU6050_FetchData(MPU6050_t *mpu)
{
	uint8_t buf[14];
	int16_t raw[7];

	MPU6050_ReadBytes(mpu, MPU6050_ACCEL_OUT_REGISTER, buf, 14);

	for (int i = 0; i < 7; i++)
		raw[i] = (int16_t)(buf[2*i] << 8 | buf[2*i+1]);

	mpu->accX  = (float)raw[0] / mpu->acc_lsb_to_g - mpu->accXoffset;
	mpu->accY  = (float)raw[1] / mpu->acc_lsb_to_g - mpu->accYoffset;
	mpu->accZ  = (float)raw[2] / mpu->acc_lsb_to_g - mpu->accZoffset;

	mpu->gyroX = (float)raw[4] / mpu->gyro_lsb_to_degsec - mpu->gyroXoffset;
	mpu->gyroY = (float)raw[5] / mpu->gyro_lsb_to_degsec - mpu->gyroYoffset;
	mpu->gyroZ = (float)raw[6] / mpu->gyro_lsb_to_degsec - mpu->gyroZoffset;
}

/* ---------- UPDATE (FILTER) ---------- */


void MPU6050_Update(MPU6050_t *mpu)
{
	MPU6050_FetchData(mpu);

	uint32_t now = HAL_GetTick();
	float dt = (now - mpu->prevTick) / 1000.0f;
	mpu->prevTick = now;

	// ACC angles
	float sgZ = (mpu->accZ < 0.0f) ? -1.0f : 1.0f;

	mpu->angleAccX = atan2f(
			mpu->accY,
			sgZ * sqrtf(mpu->accZ * mpu->accZ + mpu->accX * mpu->accX)
	) * RAD_2_DEG;

	mpu->angleAccY = -atan2f(
			mpu->accX,
			sqrtf(mpu->accZ * mpu->accZ + mpu->accY * mpu->accY)
	) * RAD_2_DEG;

	// Complementary filter
	mpu->angleX =
			mpu->filterGyroCoef * (mpu->angleX + mpu->gyroX * dt) +
			(1.0f - mpu->filterGyroCoef) * mpu->angleAccX;

	mpu->angleY =
			mpu->filterGyroCoef * (mpu->angleY + mpu->gyroY * dt) +
			(1.0f - mpu->filterGyroCoef) * mpu->angleAccY;

	mpu->angleZ += mpu->gyroZ * dt;

}


void MPU6050_CalcOffsets(MPU6050_t *mpu, uint8_t calcGyro, uint8_t calcAcc)
{
	float accX = 0, accY = 0, accZ = 0;
	float gyroX = 0, gyroY = 0, gyroZ = 0;

	/* Reset offsets before calibration */
	if (calcGyro)
	{
		mpu->gyroXoffset = 0.0f;
		mpu->gyroYoffset = 0.0f;
		mpu->gyroZoffset = 0.0f;
	}

	if (calcAcc)
	{
		mpu->accXoffset = 0.0f;
		mpu->accYoffset = 0.0f;
		mpu->accZoffset = 0.0f;
	}

	/* Let sensor settle */
	HAL_Delay(200);

	for (int i = 0; i < CALIB_OFFSET_NB_MES; i++)
	{
		MPU6050_FetchData(mpu);

		if (calcAcc)
		{
			accX += mpu->accX;
			accY += mpu->accY;
			accZ += (mpu->accZ - 1.0f);   // gravity compensation
		}

		if (calcGyro)
		{
			gyroX += mpu->gyroX;
			gyroY += mpu->gyroY;
			gyroZ += mpu->gyroZ;
		}

		HAL_Delay(2);   // ~500 Hz like Arduino
	}

	if (calcAcc)
	{
		mpu->accXoffset = accX / CALIB_OFFSET_NB_MES;
		mpu->accYoffset = accY / CALIB_OFFSET_NB_MES;
		mpu->accZoffset = accZ / CALIB_OFFSET_NB_MES;
	}

	if (calcGyro)
	{
		mpu->gyroXoffset = gyroX / CALIB_OFFSET_NB_MES;
		mpu->gyroYoffset = gyroY / CALIB_OFFSET_NB_MES;
		mpu->gyroZoffset = gyroZ / CALIB_OFFSET_NB_MES;
	}
}


