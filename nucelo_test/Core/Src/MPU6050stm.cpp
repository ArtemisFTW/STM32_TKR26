#include "MPU6050stm.h"
#include <math.h>

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


MPU6050::MPU6050(I2C_HandleTypeDef *hi2c){
	i2c=hi2c;
}

bool MPU6050::begin(){

}


int8_t MPU6050::write_i2c(uint8_t reg,uint8_t data){
	return HAL_I2C_Mem_Write(i2c, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

int8_t MPU6050::read_i2c(uint8_t reg,uint8_t data){
	return HAL_I2C_Mem_Read(i2c, addr << 1, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}
