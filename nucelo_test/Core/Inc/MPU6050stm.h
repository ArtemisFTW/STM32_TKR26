#pragma once

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

//device address
#define addr 0x68

//registers
#define smplrt_div 0x19
#define config 0x1a
#define gyro_config 0x1b
#define accel_config 0x1c
#define pwr_mgmt 0x6b  //////power management register
#define gyro_out 0x43
#define accel_out 0x3b

///constants
#define rad_2_deg 57.29578	//[deg/rad]
#define calib_offset 500
#define temp_lsb_2_deg 340.0 //[bit/celcius]
#define temp_lsb_offset 12412.0
#define default_gyro_coeff 0.98

class MPU6050{
public:
	explicit MPU6050(I2C_HandleTypeDef *hi2c);
	bool begin();

private:
	int8_t write_i2c(uint8_t reg,uint8_t data);
	int8_t read_i2c(uint8_t reg,uint8_t data);
	I2C_HandleTypeDef *i2c;

};

