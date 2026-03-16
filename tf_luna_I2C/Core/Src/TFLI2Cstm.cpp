#include "TFLI2Cstm.h"

#include <stdint.h>
#include <stdbool.h>
#include<stm32f4xx.h>

extern I2C_HandleTypeDef hi2c1;

// Constructor / Destructor
TFLI2C::TFLI2C() {}
TFLI2C::~TFLI2C() {}

// ------------------------------------------------
//              GET DATA FROM THE DEVICE
// ------------------------------------------------
bool TFLI2C::getData(int16_t &dist, int16_t &flux, int16_t &temp, uint8_t addr)
{
    tfStatus = TFL_READY;

    uint8_t idx = 0;
    for (uint8_t reg = TFL_DIST_LO; reg <= TFL_TEMP_HI; reg++, idx++)
    {
        if (!readReg(reg, addr)) return false;
        dataArray[idx] = regReply;
    }

    dist = (int16_t)((dataArray[1] << 8) | dataArray[0]);
    flux = (int16_t)((dataArray[3] << 8) | dataArray[2]);
    temp = (int16_t)((dataArray[5] << 8) | dataArray[4]);

    if (flux < (int16_t)100)
    {
        tfStatus = TFL_WEAK;
        return false;
    }
    else if (flux == (int16_t)0xFFFF)
    {
        tfStatus = TFL_STRONG;
        return false;
    }

    tfStatus = TFL_READY;
    return true;
}

// Short version
bool TFLI2C::getData(int16_t &dist, uint8_t addr)
{
    int16_t flux, temp;
    return getData(dist, flux, temp, addr);
}

// ------------------------------------------------
//              EXPLICIT COMMANDS
// ------------------------------------------------
bool TFLI2C::Get_Time(uint16_t &tim, uint8_t adr)
{
    uint8_t *p_tim = (uint8_t *)&tim;

    if (!readReg(TFL_TICK_LO, adr)) return false;
    p_tim[0] = regReply;

    if (!readReg(TFL_TICK_HI, adr)) return false;
    p_tim[1] = regReply;

    return true;
}

bool TFLI2C::Get_Prod_Code(uint8_t *p_cod, uint8_t adr)
{
    for (uint8_t i = 0; i < 14; i++)
    {
        if (!readReg((uint8_t)(0x10 + i), adr)) return false;
        p_cod[i] = regReply;
    }
    return true;
}

bool TFLI2C::Get_Firmware_Version(uint8_t *p_ver, uint8_t adr)
{
    for (uint8_t i = 0; i < 3; i++)
    {
        if (!readReg((uint8_t)(0x0A + i), adr)) return false;
        p_ver[i] = regReply;
    }
    return true;
}

bool TFLI2C::Save_Settings(uint8_t adr)
{
    return writeReg(TFL_SAVE_SETTINGS, adr, 1);
}

bool TFLI2C::Soft_Reset(uint8_t adr)
{
    return writeReg(TFL_SOFT_RESET, adr, 2);
}

bool TFLI2C::Set_I2C_Addr(uint8_t adrNew, uint8_t adr)
{
    return writeReg(TFL_SET_I2C_ADDR, adr, adrNew);
}

// ✅ FIXED LOGIC
bool TFLI2C::Set_Enable(uint8_t adr)
{
    return writeReg(TFL_DISABLE, adr, 0);
}

bool TFLI2C::Set_Disable(uint8_t adr)
{
    return writeReg(TFL_DISABLE, adr, 1);
}

bool TFLI2C::Set_Frame_Rate(uint16_t &frm, uint8_t adr)
{
    uint8_t *p_frm = (uint8_t *)&frm;

    if (!writeReg(TFL_FPS_LO, adr, p_frm[0])) return false;
    if (!writeReg(TFL_FPS_HI, adr, p_frm[1])) return false;

    return true;
}

bool TFLI2C::Get_Frame_Rate(uint16_t &frm, uint8_t adr)
{
    uint8_t *p_frm = (uint8_t *)&frm;

    if (!readReg(TFL_FPS_LO, adr)) return false;
    p_frm[0] = regReply;

    if (!readReg(TFL_FPS_HI, adr)) return false;
    p_frm[1] = regReply;

    return true;
}

bool TFLI2C::Hard_Reset(uint8_t adr)
{
    return writeReg(TFL_HARD_RESET, adr, 1);
}

bool TFLI2C::Set_Cont_Mode(uint8_t adr)
{
    return writeReg(TFL_SET_TRIG_MODE, adr, 0);
}

bool TFLI2C::Set_Trig_Mode(uint8_t adr)
{
    return writeReg(TFL_SET_TRIG_MODE, adr, 1);
}

bool TFLI2C::Set_Trigger(uint8_t adr)
{
    return writeReg(TFL_TRIGGER, adr, 1);
}

// ------------------------------------------------
//          LOW-LEVEL REGISTER ACCESS
// ------------------------------------------------
bool TFLI2C::readReg(uint8_t nmbr, uint8_t addr)
{
    if (HAL_I2C_Mem_Read(&hi2c1,
                         addr << 1,
                         nmbr,
                         I2C_MEMADD_SIZE_8BIT,
                         &regReply,
                         1,
                         100) != HAL_OK)
    {
        tfStatus = TFL_I2CREAD;
        return false;
    }

    return true;
}

bool TFLI2C::writeReg(uint8_t nmbr, uint8_t addr, uint8_t data)
{
    if (HAL_I2C_Mem_Write(&hi2c1,
                          addr << 1,
                          nmbr,
                          I2C_MEMADD_SIZE_8BIT,
                          &data,
                          1,
                          100) != HAL_OK)
    {
        tfStatus = TFL_I2CWRITE;
        return false;
    }

    return true;
}
