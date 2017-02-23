#ifndef __I2C_C
#define __I2C_C

#ifdef __cplusplus
 extern "C" {
#endif 

#define I2c_DLY_TIME        (2)
#define I2C_RETRY_MAX       (254)

extern  stI2cGpioDef stI2cGpioInfo[];

#define SCL_PORT(ch)        (stI2cGpioInfo[ch].GpioSCL.GpioPort)
#define SCL_PIN(ch)         (stI2cGpioInfo[ch].GpioSCL.GpioPin)
#define SDA_PORT(ch)        (stI2cGpioInfo[ch].GpioSDA.GpioPort)
#define SDA_PIN(ch)         (stI2cGpioInfo[ch].GpioSDA.GpioPin)


#define I2cSetScl(ch)      GpioSetBit(SCL_PORT(ch), SCL_PIN(ch));
#define I2cClrScl(ch)      GpioClrBit(SCL_PORT(ch), SCL_PIN(ch));
#define I2cSetSda(ch)      GpioSetBit(SDA_PORT(ch), SDA_PIN(ch));
#define I2cClrSda(ch)      GpioClrBit(SDA_PORT(ch), SDA_PIN(ch));
void I2cStop(enI2cChDef ch);

stI2cGpioDef stI2cGpioInfo[] = 
{
    /* channel 0*/
    {
        /*SCL*/
        {   /*port  pin  RccEn*/
            GPIOC,  GpioPin0,   RCC_APB2EN(GPIOC),
        },        
        /*SDA*/
        {
            GPIOC,  GpioPin1,   RCC_APB2EN(GPIOC),
        },
    },
    /* channel 1*/
    {
        /*SCL*/
        {   /*port  pin  RccEn*/
        GPIOB,  GpioPin10,   RCC_APB2EN(GPIOB),
        },        
        /*SDA*/
        {
        GPIOB,  GpioPin11,   RCC_APB2EN(GPIOB),
        },
    }
};
/*
    约定:
        该文件中的函数，除init waitACK和stop函数外，其他函数一定以
        拉低scl外加延时结尾
*/
void I2cInit(enI2cChDef ch)
{
    stI2cGpioDef *Gpio = &stI2cGpioInfo[ch];
    stGpioDef    GpioSCL = Gpio->GpioSCL;//I2c scl pin info
    stGpioDef    GpioSDA = Gpio->GpioSDA;//I2c sda pin info
    GpioInitDef  GPIO_InitStruct = {GpioPin5};

    /*rcc init*/
    RCC->APB2ENR  |= (GpioSCL.RccEn) | (GpioSDA.RccEn);

    /*gpio init*/
    GPIO_InitStruct.GpioPin   = GpioSCL.GpioPin;    
    GPIO_InitStruct.GPIO_Speed = GpioSpeed50MHz;
    GPIO_InitStruct.GPIO_Mode  = GpioModeOutPP;
    GpioInit(GpioSCL.GpioPort, &GPIO_InitStruct);
    I2cSetSda(ch);

    GPIO_InitStruct.GpioPin   = GpioSDA.GpioPin;    
    GpioInit(GpioSDA.GpioPort, &GPIO_InitStruct);
    I2cSetScl(ch);
    DelayUs(I2c_DLY_TIME);
    return;
}

void I2cCfgSdaDir(enI2cChDef ch, GpioDirDef enDir)
{
    GpioInitDef GPIO_InitStruct;
    bool bIsInupt = (GpioIn == enDir);
    
    GPIO_InitStruct.GpioPin   = SDA_PIN(ch);    
    GPIO_InitStruct.GPIO_Speed = bIsInupt ? GpioInput : GpioSpeed50MHz;
    GPIO_InitStruct.GPIO_Mode  = bIsInupt ? GpioModeInFloat : GpioModeOutPP;
    GpioInit(SDA_PORT(ch), &GPIO_InitStruct);
    return;
}

//wait ack 0:success 1:time out 
uint8 I2cWaitAck(enI2cChDef ch)
{
    uint8  ucCnt = 0;
    uint16 err = 0;

    I2cClrSda(ch);
    DelayUs(I2c_DLY_TIME);
    I2cCfgSdaDir(ch, GpioIn);
    I2cSetScl(ch);
    while(GpioReadBit(SDA_PORT(ch), SDA_PIN(ch)))
    {
        ucCnt++;
        DelayUs(I2c_DLY_TIME);
        if (ucCnt > I2C_RETRY_MAX)
        {
            err = 1;
            goto ErrReturn;
        }
    }
ErrReturn:
    I2cClrScl(ch);
    DelayUs(I2c_DLY_TIME);
    I2cCfgSdaDir(ch, GpioOut);
    if (err)
    {
        PrintErr(I2cWaitAck, 0, 1);
    }
    return err;
}
void I2cSendAck(enI2cChDef ch, bool bAck)
{
    I2cCfgSdaDir(ch, GpioOut);
    if (bAck)
    {
        I2cClrSda(ch);
    }
    else
    {
        I2cSetSda(ch);
    }
    DelayUs(I2c_DLY_TIME);
    I2cSetScl(ch);
    DelayUs(I2c_DLY_TIME);
    I2cClrScl(ch);
    DelayUs(I2c_DLY_TIME);
    I2cClrSda(ch);
    DelayUs(I2c_DLY_TIME);
    return;
}


void I2cStart(enI2cChDef ch, uint8 addr, enDataDirDef rw)
{
    uint8 ucTmp = 0;

    if (enRead == rw)
    {//
        ucTmp = 1;
    }
    else
    {
        ucTmp = 0;
    }
    I2cSetScl(ch);
    DelayUs(I2c_DLY_TIME);
    I2cSetSda(ch);
    DelayUs(I2c_DLY_TIME);
    I2cClrSda(ch);
    DelayUs(I2c_DLY_TIME);
    I2cClrScl(ch);
    DelayUs(I2c_DLY_TIME);
    I2cWriteByte(ch, (addr << 1) | ucTmp);
    return;
}

void I2cStop(enI2cChDef ch)
{
    I2cClrSda(ch);
    DelayUs(I2c_DLY_TIME);
    I2cSetScl(ch);
    DelayUs(I2c_DLY_TIME);
    I2cSetSda(ch);
    DelayUs(I2c_DLY_TIME);
    return;
}
uint16 I2cWriteByte(enI2cChDef ch, uint8 ucData)
{
    uint8  i = 0;
    uint8  ucTmp = 1 << 7;
    uint16 err = 0;

    for(i = 0; i < 8; i++)
    {
        if (ucData & ucTmp)
        {
            I2cSetSda(ch);
        }
        else
        {
            I2cClrSda(ch);
        }
        ucTmp >>= 1;
        DelayUs(I2c_DLY_TIME);
        I2cSetScl(ch);
        DelayUs(I2c_DLY_TIME);
        I2cClrScl(ch);
        DelayUs(I2c_DLY_TIME);
        I2cClrSda(ch);
        DelayUs(I2c_DLY_TIME);
    }
    err = I2cWaitAck(ch);
    if (err)
    {
        PrintErr(I2cWriteByte, 0, 1);
    }
    return err;
}
uint16 I2cWrite(enI2cChDef ch, uint8 SlaveAddr, uint8 *pucData, uint16 usLen)
{
    uint16 err = 0;
    uint16 i   = 0;

    I2cStart(ch, SlaveAddr, enWrite);
    for(i = 0; i < usLen; i++)
    {
        err = I2cWriteByte(ch, pucData[i]);
    }
    I2cStop(ch);
    if (err)
    {
        PrintErr(I2cWrite, 0, 1);
    }
    return err;
}

void I2cReadByte(enI2cChDef ch, uint8 *ucData, bool bAck)
{
    uint8  i = 0;

    *ucData = 0;
    I2cCfgSdaDir(ch, GpioIn);
    for(i = 0; i < 8; i++)
    {
        *ucData <<= 1;
        I2cSetScl(ch);
        DelayUs(I2c_DLY_TIME);
        if (GpioReadBit(SDA_PORT(ch), SDA_PIN(ch)))
        {
            *ucData |= 1;
        }
        I2cClrScl(ch);
        DelayUs(I2c_DLY_TIME);
    }
    I2cSendAck(ch, bAck);
    return;
}



void I2cRead(enI2cChDef ch, uint8 SlaveAddr, uint8 *pucData, uint16 usLen)
{
    uint16 i   = 0;

    I2cStart(ch, SlaveAddr, enRead);
    for(i = 0; i < (usLen - 1); i++)
    {
        I2cReadByte(ch, &pucData[i], TRUE);
    }
    I2cReadByte(ch, &pucData[i], FALSE);
    I2cStop(ch);
    return;
}

#ifdef __cplusplus
}
#endif
#endif

