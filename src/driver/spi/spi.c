#ifndef __SPI_C
#define __SPI_C

#ifdef __cplusplus
 extern "C" {
#endif 

#define SPI_CH_SZ       (SpiMax + 1)
extern SPI_TypeDef *const astSpiChConvert[SPI_CH_SZ];

#define SPI(ch)         (astSpiChConvert[(ch)])
//#define HARD_NSS_EN     1

#define SPI_DBG         1

#define SPI_ERR_MSK     (I2S_FLAG_UDR | SPI_FLAG_CRCERR | SPI_FLAG_MODF | SPI_I2S_FLAG_OVR)


#define SPI_RETRY_MAX   (100)

typedef const struct{
    stGpioDef       GpioMOSI;
    stGpioDef       GpioMISO;
    stGpioDef       GpioSCK;
    stGpioDef       GpioCS;
}stSpiGpioDef;

typedef void (*pAfCfgFunc)(void);

typedef const struct{
    SPI_TypeDef *  SPI;
    stSpiGpioDef   SpiGpio;//GPIO_Pin_5 etc.
    uint32_t       RccEn;//RCC_APB2Periph_SPI1 etc.
    pAfCfgFunc     pRemapCfg;//Remapcfg
}stSpiInfoDef;

pSpiReadFunc apSpiReadFunc[SPI_CH_SZ] = {0};

stSpiInfoDef astSpiInfo[] = {
    {
        /*SPI 1*/
        SPI1,  
        /*GPIO                                */
        {
            {GPIOA, GpioPin7, RCC_APB2EN(GPIOA),}, //MOSI
            {GPIOA, GpioPin6, RCC_APB2EN(GPIOA),}, //MISO
            {GPIOA, GpioPin5, RCC_APB2EN(GPIOA),}, //SCK
            {GPIOA, GpioPin3, RCC_APB2EN(GPIOA),}, //CS
        }, 
        /*RccEn*/
        RCC_APB2EN(SPI1),
        /*pRemapCfg*/
        NULL,
    },
    {
        /*SPI 2*/
        SPI2,  
        /*GPIO*/
        {
            {GPIOB, GpioPin15, RCC_APB2EN(GPIOB),}, //MOSI
            {GPIOB, GpioPin14, RCC_APB2EN(GPIOB),}, //MISO
            {GPIOB, GpioPin13, RCC_APB2EN(GPIOB),}, //SCK
            {GPIOB, GpioPin12, RCC_APB2EN(GPIOB),}, //CS
        }, 
        /*RccEn*/
        RCC_APB1EN(SPI2),
        /*pRemapCfg*/
        NULL,
    },
    {
        /*SPI 3*/
        SPI3,  
            /*GPIO                                */
        {
            {GPIOB, GpioPin5, RCC_APB2EN(GPIOB),}, //MOSI
            {GPIOB, GpioPin4, RCC_APB2EN(GPIOB),}, //MISO
            {GPIOB, GpioPin3, RCC_APB2EN(GPIOB),}, //SCK
            {GPIOB, GpioPin12, RCC_APB2EN(GPIOB),}, //CS
        }, 
        /*RccEn*/
        RCC_APB1EN(SPI3),
        /*pRemapCfg*/
        NULL,
    }
};
#define SpiClrCs(ch)            do{GpioClrBit(astSpiInfo[ch].SpiGpio.GpioCS.GpioPort, astSpiInfo[ch].SpiGpio.GpioCS.GpioPin);}while(0)
#define SpiSetCs(ch)            do{GpioSetBit(astSpiInfo[ch].SpiGpio.GpioCS.GpioPort, astSpiInfo[ch].SpiGpio.GpioCS.GpioPin);}while(0)

SPI_TypeDef * const astSpiChConvert[SPI_CH_SZ] = {
    SPI1,
    SPI2,
    SPI3,
};

void SpiGpioInit(stSpiGpioDef   *SpiGpio)
{
    stGpioDef *GpioMOSI = &SpiGpio->GpioMOSI;
    stGpioDef *GpioMISO = &SpiGpio->GpioMISO;
    stGpioDef *GpioSCK  = &SpiGpio->GpioSCK;
    GpioInitDef GPIO_InitStruct;

    /*gpio rcc ENR*/ 
    RCC->APB2ENR |= GpioMOSI->RccEn | GpioMISO->RccEn | GpioSCK->RccEn;

    GPIO_InitStruct.GPIO_Speed  = GpioSpeed50MHz;
    GPIO_InitStruct.GPIO_Mode   = GpioModeAfpp;
    GPIO_InitStruct.GpioPin     = GpioSCK->GpioPin;
    GpioInit(GpioSCK->GpioPort, &GPIO_InitStruct);
    
    GPIO_InitStruct.GpioPin     = GpioMOSI->GpioPin;
    GpioInit(GpioMOSI->GpioPort, &GPIO_InitStruct);

    GPIO_InitStruct.GpioPin     = GpioMISO->GpioPin;
    GPIO_InitStruct.GPIO_Mode   = GpioModeInFloat;
    GPIO_InitStruct.GPIO_Speed  = GpioInput;
    GpioInit(GpioMISO->GpioPort, &GPIO_InitStruct);
    
    return;
}

void SpiGpioCsInit(stGpioDef *GpioCs)
{
    GpioInitDef GPIO_InitStruct;

    /*gpio rcc ENR*/ 
    RCC->APB2ENR |= GpioCs->RccEn;

    GPIO_InitStruct.GPIO_Mode   = GpioModeOutPP;    
    GPIO_InitStruct.GPIO_Speed  = GpioSpeed50MHz;
    GPIO_InitStruct.GpioPin    = GpioCs->GpioPin;
    GpioInit(GpioCs->GpioPort, &GPIO_InitStruct);

    GpioSetBit(GpioCs->GpioPort, GpioCs->GpioPin);
    return;
}

void SpiRccInit(enSpiChDef enCh, uint32_t ulRccEn)
{
    RCC->APB2ENR |= RCC_APB2Periph_AFIO;
    if (Spi1 == enCh)
    {
        RCC->APB2ENR |= ulRccEn;
    }
    else
    {
        RCC->APB1ENR |= ulRccEn;
    }
    return;
}

void SpiInit(stSpiInitDef *stCh)
{
    enSpiChDef      enCh    = stCh->Ch;//channel
    stSpiInfoDef * stSpiInfo  = &astSpiInfo[enCh];

    //spi RCC init
    SpiRccInit(enCh, stSpiInfo->RccEn);
 
    //GPIO init
    {
        stSpiGpioDef     *pstSpiGpio  = &stSpiInfo->SpiGpio;
        stGpioDef        *pstGpioCs   = &pstSpiGpio->GpioCS;
        pAfCfgFunc       pRemapCfg = stSpiInfo->pRemapCfg;//

        if (pRemapCfg)
        {
            pRemapCfg();
        }
        else
        {
            SpiGpioInit(pstSpiGpio);//初始化除cs外的所有spi引脚
        }
        SpiGpioCsInit(pstGpioCs);//cs脚初始化
    }
    //SPI init
    {
        SPI_TypeDef     *SPI             = stSpiInfo->SPI;
        SPI_InitTypeDef *SPI_InitStruct = stCh->SPI_InitStruct;

        SPI_Init(SPI, SPI_InitStruct);
        SPI_SSOutputCmd(SPI, DISABLE);// hard NSS disable
    }

    apSpiReadFunc[enCh] = stCh->pSpiReadFunc;
    return;
}

uint16 SpiWaitActEnd(SPI_TypeDef *SPIx, enDataDirDef enRw)
{
    bool      bIsRead  = (enRw == enRead);//判断是否为read
    uint8     ucFlgMsk = bIsRead ? SPI_I2S_FLAG_RXNE : SPI_I2S_FLAG_TXE;
    uint16    err = 0;
    uint16    usTmp = 0;     
    uint8     ucStep = 0;
    uint8     ucRetryCnt = SPI_RETRY_MAX;
    
    while((0 == usTmp) && ucRetryCnt)
    {
        usTmp = SPIx->SR;
        ucRetryCnt--;
        err = usTmp & SPI_ERR_MSK;
        if (err)
        {
            ucStep = 0;
            goto ErrReturn;
        }
        usTmp &= ucFlgMsk;
    }
    if (0 == ucRetryCnt)
    {//超时
        err = 0xff;
    }
ErrReturn:
    if (err)
    {
        PrintErr(SpiWaitActEnd, ucStep, err);
    }
    return err; 
}

uint16 SpiRwByte(SPI_TypeDef *SPIx, uint8 const ucSend, uint8 *const pucRecv)
{
    uint16 err = 0;
    uint8  ucStep = 0;
    
    err = SpiWaitActEnd(SPIx, enWrite);    
    if (err)
    {
        ucStep = 0;
        goto ErrReturn;
    }
    SPIx->DR = ucSend;
    err = SpiWaitActEnd(SPIx, enRead);    
    if (err)
    {
        ucStep = 0;
        goto ErrReturn;
    }
    *pucRecv   = SPIx->DR;
    //DelayUs(20);
    return 0;
ErrReturn:
    PrintErr(SpiWriteByte, ucStep, err);
    return err;
}

uint16 SpiDataWrite(enSpiChDef enCh, uint8 const *const pucWrite, uint16 const usDataSz)
{
    uint16 i = 0;
    uint16 err = 0;
    uint8  ucStep = 0;
    uint8  ucTmp = 0;
    
    SPI_Cmd(SPI(enCh), ENABLE);
    SpiClrCs(enCh);
    for (i = 0; i < usDataSz; i++)
    {
        err = SpiRwByte(SPI(enCh), pucWrite[i], &ucTmp);
        if(err)
        {
            ucStep = 0;
            goto ErrReturn;
        }
    }
    while(SET == SPI_I2S_GetFlagStatus(SPI(enCh), SPI_I2S_FLAG_BSY));//Busy
    SPI_Cmd(SPI(enCh), DISABLE);
    DelayUs(20);
    SpiSetCs(enCh);
ErrReturn:
    if (err)
    {
        PrintErr(SpiDataWrite, ucStep, err);
    }
    return err;
}

uint16 SpiDataRead(enSpiChDef enCh, uint8 *const pucRecv, uint16 usRecvLen, uint8 const *const pucSend, uint16 usSendLen)
{
    if (apSpiReadFunc[enCh])
    {
        return apSpiReadFunc[enCh](enCh, pucRecv, usRecvLen, pucSend, usSendLen);
    }
    else
    {
        return 0;
    }
}

uint16 SpiDataWriteByte(enSpiChDef enCh, uint8 const ucWrite)
{
    uint16 err = 0;
    uint8  ucStep = 0;
    uint8  ucTmp = 0;
    
    SPI_Cmd(SPI(enCh), ENABLE);
    SpiClrCs(enCh);
    err = SpiRwByte(SPI(enCh), ucWrite, &ucTmp);
    if(err)
    {
        ucStep = 0;
        goto ErrReturn;
    }
    while(SET == SPI_I2S_GetFlagStatus(SPI(enCh), SPI_I2S_FLAG_BSY));//Busy
    SPI_Cmd(SPI(enCh), DISABLE);
    DelayUs(20);
    SpiSetCs(enCh);
ErrReturn:
    if (err)
    {
        PrintErr(SpiDataWriteByte, ucStep, err);
    }
    return err;
}

/*
MFRC522读操作
注意:usSendLen为0时将重复发送pucSend[0]
*/
uint16 RC522SpiRead(enSpiChDef enCh, uint8 *const pucRecv, uint16 usRecvLen, uint8 const *const pucSend, uint16 usSendLen)
{
    uint16 i       = 0;
    uint16 err     = 0;
    uint8  ucStep  = 0;
    uint8  ucTmp   = 0;
    uint8  ucSend  = 0;

    SpiClrCs(enCh);
    SPI_Cmd(SPI(enCh), ENABLE);
    err = SpiRwByte(SPI(enCh), pucSend[i++], &ucTmp);//dummy read
    if (err)
    {
        ucStep = 0;
        goto ErrReturn;
    }
    for (i = 1; i < usRecvLen; i++)
    {
        ucSend = ((0 == usSendLen) ? pucSend[0] : pucSend[i]);
        err = SpiRwByte(SPI(enCh), ucSend, &pucRecv[i - 1]);    
        if (err)
        {
            ucStep = 1;
            goto ErrReturn;
        }
    }
    
    err = SpiRwByte(SPI(enCh), 0, &pucRecv[usRecvLen - 1]);//读取最后一个byte
    if (err)
    {
     ucStep = 2;
     goto ErrReturn;
    }
    while(SET == SPI_I2S_GetFlagStatus(SPI(enCh), SPI_I2S_FLAG_BSY));//Busy
    SPI_Cmd(SPI(enCh), DISABLE);
    DelayUs(20);
    SpiSetCs(enCh);
    return 0;
ErrReturn:
    PrintErr(RC522SpiRead, ucStep, err);
    return err;
}


/*
    规则:
    先发送pucSend为头的usSendLen个字节
    然后接收usRecvLen个字节放入pucRecv
*/
uint16 W25Q64SpiRead(enSpiChDef enCh, uint8 *const pucRecv, uint16 usRecvLen, uint8 const *const pucSend, uint16 usSendLen)
{
    uint16 i       = 0;
    uint16 err     = 0;
    uint8  ucStep  = 0;
    uint8  ucTmp   = 0;

    SpiClrCs(enCh);
    SPI_Cmd(SPI(enCh), ENABLE);
    DelayUs(20);

    for (i = 0; i < usSendLen; i++)
    {
        err = SpiRwByte(SPI(enCh), pucSend[i], &ucTmp);//发送
        if (err)
        {
            ucStep = 0;
            goto ErrReturn;
        }
    }

    for (i = 0; i < usRecvLen; i++)
    {        
        err = SpiRwByte(SPI(enCh), pucSend[i], &pucRecv[i]);    
        if (err)
        {
            ucStep = 1;
            goto ErrReturn;
        }
    }
    while(SET == SPI_I2S_GetFlagStatus(SPI(enCh), SPI_I2S_FLAG_BSY));//Busy
    SPI_Cmd(SPI(enCh), DISABLE);
    DelayUs(20);
    SpiSetCs(enCh);
ErrReturn:
    if (err)
    {
        PrintErr(W25Q64SpiRead, ucStep, err);
    }
    return err;
}


#ifdef __cplusplus
}
#endif
#endif

