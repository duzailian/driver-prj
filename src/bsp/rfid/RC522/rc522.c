#ifndef __RC522_C
#define __RC522_C

#ifdef __cplusplus
 extern "C" {
#endif 

#include "rc522_reg.h"
#include "rc522_cmd.h"
#include "middle.h"

/*使能/除能MFRC522中断*/
#define RC522_INT_EN        0

#define RC522_RETRY_MAX   (110)//重试次数

/*Timer Para*/
/*
注意:RC522_TIMER_PRES 必须保证满足延时要求
*/
#define RC522_TIMER_PRES        (100)//Timer Prescaler 0 and 4095 
#if (0 == ((13560) / (RC522_TIMER_PRES * 2 + 1)))
#err "Timer Prescaler Err!"
#else
/*Reload Val Per Ms*/
#define RC522_RELOAD_MS         ((13.56 * 1000) / (RC522_TIMER_PRES * 2 + 1) - 1)
#endif

#define RC522_READ_FLG      (1 << 7)
#define RC522_WRITE_FLG     (0)

#define RC522_BUFFER_SZ     64//FIFO buffer size

#define RC522_SPI_CH        (Spi1)


/*RC522 GPIO*/
/*interrupt pin(如果更改interrupt pin RC522_INT_IRQn必须同步更改)*/
#define RC522_INT_PORT      (GPIOA)
#define RC522_INT_PIN       (GpioPin0)
#define RC522_INT_LINE      (EXTI_Line0)
#define RC522_INT_ENR       (RCC_APB2EN(GPIOA))

/*rest pin*/
#define RC522_RST_PORT      (GPIOA)
#define RC522_RST_PIN       (GpioPin4)
#define RC522_RST_ENR       (RCC_APB2EN(GPIOA))

/*SPI Baud rate*/
#define RC522_BR            (SPI_BaudRatePrescaler_8)//spi 72M / 8

/*WithOut Collision*/
#define NO_COLL             (0x00)
#define RANGE_COLL          (0xff)// position of the collision is out of the range

#if !RC522_INT_EN
/*定义查询模式下的flg*/
typedef enum{
FlgTx,
FlgRx,
FlgIdle,
FlgTimer,
FlgCRC,
FlgMFCrypto1,
}enRc522FlgDef;
#endif

static uint8 Rc522WaitFlg(enRc522FlgDef enFlg);
static void Rc522PwrSw(enOnOffDef OnOff);
static uint8 Rc522ReadReg(uint8 const addr);
static void Rc522WriteReg(uint8 const addr, uint8 ucData);
static void Rc522ClrIntrBuffer(void);
static void Rc522Rst(void);

#if RC522_DBG
static void Rc522SelfTest(void);
#endif
static uint8 Rc522GetRxBitsCnt(void);
static bool Rc522SendCmd(uint8 const ucCmd);

/*获取addr byte*/
#define RC522_GET_ADDR_BYTE(wr, addr)        (wr | ((addr) << 1))

#define Rc522IrqClr(cmd)                     do{\
                                                    Rc522WriteReg(RegComReq, 0x00);\
                                                    Rc522WriteReg(RegDivReq, 0x00);\
                                                }while(0)



/* Timer Start/Stop */
#define Rc522StartT()                         do{Rc522WriteReg(RegControl, TStartNow);}while(0)
#define Rc522StopT()                          do{Rc522WriteReg(RegControl, TStopNow);}while(0)
#define Rc522GetTReload(ms)                  ((ms) * RC522_RELOAD_MS)

/******************************RC522 Cfg Para************************************/


/************************************Rc522 sta/err check*********************/
/*
功能:
    1.检查Rc522 Error Register的相应Bits是否置位
参数:   
    1.ucMark: 要检查的Bits，如:CollErr etc.
返回值: 
    1.TRUE:  要检查的Bits已置位
    2.FALSE: 要检查的Bits未置位
输入:   void
输出:   void
备注:   无
注意:   
    1.执行命令可以清除除TempErr外的所有错误位
 */
static uint8 Rc522ChckErr(uint8 ucMark)
{
    uint8 ucTmp = 0;
	
    ucTmp = Rc522ReadReg(RegErr);

    return (ucTmp & ucMark) ? TRUE : FALSE;
}

/*
功能:
    1.判断Rc522接收来自PICC的数据时是否有数据碰撞发生
参数:   void
返回值: 
    TRUE:  有碰撞
    FALSE: 无碰撞
输入:   void
输出:   void
备注:   void
注意:   void
*/
bool Rc522IsColl(void)
{
    return  Rc522ChckErr(CollErr) ? TRUE : FALSE;
}

/*
功能:
    1.获取碰撞bit所在位置(1-32)
参数:   
    1.ucMark: 要检查的Bits，如:CollErr etc.
返回值: 
    碰撞bit所在位置(返回NO_COLL表示没有碰撞, RANGE_COLL表示碰撞超范围)  
输入:   void
输出:   void
备注:   void
注意:   void
 */
static uint8 Rc522GetCollBitPos(void)
{
    uint8 ucPos = NO_COLL;
    uint8 ucTmp = 0;
    
    if (Rc522IsColl())
    {
        ucTmp = Rc522ReadReg(RegColl);
        if(ucTmp & CollPosNotValid)
        {// position of the collision is out of the range
            ucPos = RANGE_COLL;
        }
        else
        {
            ucPos = ucTmp & CollPosMask;
            if (0 == ucPos)
            {
                ucPos = 32;
            }
        }
    }
    return ucPos;
}

/*
功能:
    1.获取已成功接收的Bits数
参数:
    1.pucBitCnt指向用于存储有效Bits数的变量的指针
返回值:
    TRUE:   有碰撞
    FALSE:  无碰撞
备注:   void
注意:
    1.若碰撞bit超过32bit，则*pucBitCnt为32
    2.若没有碰撞发生，*pucBitCnt = FIFO中的数据长度 * 8
*/
bool Rc522GetValidCnt(uint8 *pucBitCnt)
{
    uint8 ucPos    = Rc522GetCollBitPos();
    bool  bIsColl  = FALSE;

    if (NO_COLL == ucPos)
    {//无碰撞位
        uint8 ucRecvCnt = 0;

        ucRecvCnt = Rc522GetFifoLevel();
        *pucBitCnt = ucRecvCnt * 8;
        bIsColl = FALSE;
    }
    else if (RANGE_COLL == ucPos)
    {//碰撞位超范围
        *pucBitCnt = 32;
        bIsColl = TRUE;
    }
    else
    {
        ucPos--;//1-32 -> 0-31 碰撞位置转换为有效bits计数
        *pucBitCnt = ucPos;
        bIsColl = TRUE;
    }
    return bIsColl;
}

/************************************Rc522 Init*********************/
#if RC522_INT_EN
/*
功能:
    1.MFRC522 Interrupt Init
参数:   void
返回值: void
备注:   void
注意:   void
*/
static void Rc522IntInit(void)
{
    /* interrupt enable*/
    Rc522WriteReg(RegComlEn, (TimerIEn | IRqInv));
    Rc522WriteReg(RegDivIEn, IRQPushPull);
    return;
}

/*
功能:
    1.MFRC522 Interrupt 处理
参数:   void
返回值: void
备注:   void
注意:   void
*/
static void Rc522IntDeal(void)
{
    uint8 ucCom = 0;
    uint8 ucDiv = 0;

    ucCom = Rc522ReadReg(RegComReq);
    ucDiv = Rc522ReadReg(RegDivReq);

    if (ucCom & TimerIRq)
    {
        
    }
    if (ucCom & ErrIRq)
    {

    }
    if (ucCom & IdleIRq)
    {

    }
    if (ucCom & RxIRq)
    {

    }
    if (ucCom & TxIRq)
    {

    }
    if (ucDiv & CRCIRq)
    {

    }
    return;
}

/*
功能:
    1.MFRC522 用于中断服务函数
参数:   void
返回值: void
备注:   void
注意:   void
*/
static void Rc522Isr(void)
{
    Rc522IntDeal();
	Rc522IrqClr();
    EXTI_ClearITPendingBit(RC522_INT_LINE);//清标志
    return;
}
#else
/*
功能:
    1.MFRC522 查询模式下，不进行Interrupt初始化操作
参数:   void
返回值: void
备注:   void
注意:   void
*/
#define Rc522IntInit()       do{}while(0)

/*
功能:
    1.等待ComIrqReg、DivIrqReg或Status2寄存器中的相应Bits置位
    2.清零ComIrqReg或DivIrqReg寄存器中的对应Bits
    3.若等待超时活期间Error Register中有Bit(s)置位,则返回错误代码
参数:   void
返回值: 
    0:      success
    Others: error code
备注:   void
注意:   void
*/
static uint8 Rc522WaitFlg(enRc522FlgDef enFlg)
{
    uint8 err = 0xff;
    uint8 i   = RC522_RETRY_MAX;
    uint8 ucTmp = 0;
    uint8 ucAddr = 0;
    uint8 step = 0;
    uint8 ucFlg = 0;

    /*根据enFlg确定要操作的寄存器地址*/
    if (enFlg < FlgCRC)
    {
        ucAddr = RegComReq;
    }
    else if(FlgMFCrypto1 == enFlg)
    {
        ucAddr = RegStatus2;
    }
    else
    {
        ucAddr = RegStatus1;
    }
    /*将enFlg转换为对应的寄存器中的位标志*/
    switch(enFlg)
    {
        case FlgTx:
        {
            ucFlg = TxIRq;
            break;
        }
        case FlgRx:
        {
            ucFlg = RxIRq;
            break;
        }
        case FlgIdle:
        {
            ucFlg = IdleIRq;
            break;
        }
        case FlgTimer:
        {
            ucFlg = TimerIRq;
            break;
        }
        case FlgCRC:
        {
            ucFlg = CRCReady;
            break;
        }
        case FlgMFCrypto1:
        {
            ucFlg = MFCrypto1On;
            break;
        }
        default:
        {
            step = 0;
            err = 0xff;
            goto ErrReTurn;
        }
    }
    do 
    {
        ucTmp = Rc522ReadReg(RegComReq);
    #if RC522_DBG
        printf("RegComReq:%x\r\n", (ucTmp));
    #endif
        err = Rc522ReadReg(RegErr);
        if (ucTmp & ErrIRq)
        {
            step = 1;
            goto ErrReTurn;
        }
        ucTmp = Rc522ReadReg(ucAddr);
        if (ucTmp & ucFlg)
        {
            goto SuccessReturn;
        }
        DelayUs(100);
    }
    while(i--);    
    err = 0xfe;
    step = 2;
ErrReTurn:
    PrintErr(Rc522WaitFlg, step, err);
    return err;
SuccessReturn:
#if S50DBG
    if (i != 0xff)
    {
        printf("Rc522 TimeOut:%d\r\n", i);
    }
#endif
    Rc522WriteReg(ucAddr, ucTmp & (~ucFlg));
    return 0;
}
#endif

/*
功能:
    1.等待数据接收完成,并停止Transceive命令
    2.FIFO中的数据长度是否为0
参数:   void
返回值: 
    TRUE:  Success
    FALSE: Fail
备注:   void
注意:   
    1.本函数仅用于Transceive命令发送完成后
*/
bool Rc522WaitRxEnd(void)
{
    bool res = FALSE;

#if 0//S50DBG
    if ((0 == Rc522WaitFlg(FlgRx)) && Rc522GetFifoLevel())
#else
    if (0 == Rc522WaitFlg(FlgRx))
#endif
    {
        res = TRUE;
    }   
    DelayMs(1);//FlgRx置位后需要等一会儿才能正常获取FIFO中的数据长度
    return res;
}

/*
功能:
    1.用RC522计算CRC_A
参数:
    1.pucData 指向要计算CRC的数据包头
    2.ucLen   pucData的数据长度
返回值: 
    TRUE:  Success
    FALSE: Fail
输入:   
    1.pucData
输出:   void
备注:   void
注意:   
    1.若要发送的数据结尾包含RCR_A
*/
bool Rc522CalcCrc(uint8 *pucData, uint8 ucLen)
{
    bool bRes = FALSE;
	
    Rc522WriteFIFO(pucData, ucLen);
    Rc522SendCmd(CmdCRC);
    if(Rc522WaitFlg(FlgCRC))
    {
        bRes = FALSE;
        printf("S50 Calc Crc Err!\r\n");
        goto ErrReturn;
    }
    bRes = TRUE;
ErrReturn:
    Rc522SendCmd(CmdIdle);
    return bRes;
}

/*
功能:
    1.初始化RC522 Timer
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522TimerInit(void)
{
    Rc522WriteReg(RegTMode,      TPresHiMask   & (RC522_TIMER_PRES >> 8));
    Rc522WriteReg(RegTPrescaler, TPresLiMask   & RC522_TIMER_PRES);
    //Rc522WriteReg(RegTReloadH,   TReloadHiMask & (RC522_TIMER_RELOAD >> 8));
    //Rc522WriteReg(RegTReloadL,   TReloadLiMask & (RC522_TIMER_RELOAD));
    return;
}

/*
功能:
    1.RC522 通用寄存器Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522ComInit(void)
{
    Rc522IrqClr();
    Rc522WriteReg(RegStatus2, 0x00);
    Rc522WriteReg(RegColl, CollNoClr);//all received bits will be cleared after a collision disable
    Rc522WriteReg(RegMode, TxWaitRF | Crc6363);/*transmitter can only be started if an RF field is generated*/
    return;
}

/*
功能:
    1.RC522 发送相关寄存器Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522RxInit(void)
{
    //Rc522WriteReg(RegRxMode, RxSpeed106k | RxNoErr);
    //Rc522WriteReg(RegRxSel, 0);
    //Rc522WriteReg(RegRxThrd, 0);//defines the minimum signal strength at the decoder input that will be accepted
    Rc522WriteReg(RegMfRx, ParityDisable);
    return ;
}

/*
功能:
    1.RC522 接收相关寄存器Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522TxInit(void)
{
    //Rc522WriteReg(RegTxMode, TxSpeed106k);/*106 kBd   Tx CRC Disable */
    //Rc522WriteReg(RegTxCtrl, Tx2RFEn | Tx1RFEn);
    Rc522WriteReg(RegTxASK,  Force100ASK);
    Rc522WriteReg(RegTxSel,  DriverSel);
    return ;
}

/*
功能:
    1.RC522 Hard Rest
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522Rst(void)
{
    GpioClrBit(RC522_RST_PORT, RC522_RST_PIN);
    DelayMs(1);
    GpioSetBit(RC522_RST_PORT, RC522_RST_PIN);
    DelayMs(1);
    return;
}

/*
功能:
    1.RC522 Power On
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522PowerOn(void)
{
    Rc522Rst();
    Rc522PwrSw(enOff);
    Rc522PwrSw(enOn);
    Rc522SendCmd(CmdSoftRst);//soft rest
    Rc522RfSw(enOn);
    return;
}

/*
功能:
    1.RC522 RF相关寄存器Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522RfInit(void)
{
    Rc522WriteReg(RegRFCfg,  RxGain);
    return ;
}

/*
功能:
    1.RC522 Register Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522RegInit(void)
{
    Rc522PowerOn();
    Rc522IntInit();//中断配置
    Rc522ComInit();//通用性寄存器初始化
    Rc522RxInit();
    Rc522TxInit();
    Rc522RfInit();
    Rc522TimerInit();
    return;
}

/*
功能:
    1.RC522 SPI Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522SpiInit(void)
{
        stSpiInitDef stSpiInitInfo;
        SPI_InitTypeDef SPI_InitStruct;//used in StdPeriph_Lib

        /*Spi Init*/
        SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStruct.SPI_Mode      = SPI_Mode_Master;
        SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;
        SPI_InitStruct.SPI_CPOL      = SPI_CPOL_Low;
        SPI_InitStruct.SPI_CPHA      = SPI_CPHA_1Edge;        
        SPI_InitStruct.SPI_NSS       = SPI_NSS_Soft;
        SPI_InitStruct.SPI_BaudRatePrescaler  = RC522_BR;
        SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;
        SPI_InitStruct.SPI_CRCPolynomial  = 1;

        stSpiInitInfo.Ch             = RC522_SPI_CH;
        stSpiInitInfo.SPI_InitStruct = &SPI_InitStruct;
        stSpiInitInfo.pSpiReadFunc   = RC522SpiRead;
        SpiInit(&stSpiInitInfo);
        return;
}

/*
功能:
    1.RC522 GPIO Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522GpioInit(void)
{
    GpioInitDef GPIO_InitStruct;
#if RC522_INT_EN//中断模式
    {
        EXTI_InitTypeDef EXTI_InitStruct;
        /*interrupt pin cfg*/
        RCC->APB2ENR |= RC522_INT_ENR;
        GPIO_InitStruct.GpioPin     = RC522_INT_PIN;
        GPIO_InitStruct.GPIO_Mode   = GpioModeInFloat;
        GPIO_InitStruct.GPIO_Speed  = GpioInput;
        GpioInit(RC522_INT_PORT, &GPIO_InitStruct);

        /*interrupt cfg*/
        EXTI_InitStruct.EXTI_Line    = RC522_INT_LINE;
        EXTI_InitStruct.EXTI_LineCmd = ENABLE;
        EXTI_InitStruct.EXTI_Mode    = EXTI_Mode_Interrupt;
        EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;
        EXTI_Init(&EXTI_InitStruct);
        IrqRegester(RC522_INT_IRQn, Rc522Isr);
    }
#endif
    {
        RCC->APB2ENR |= RC522_RST_ENR;
        GPIO_InitStruct.GpioPin     = RC522_RST_PIN;
        GPIO_InitStruct.GPIO_Mode   = GpioModeOutPP;
        GPIO_InitStruct.GPIO_Speed  = GpioSpeed10MHz;
        GpioInit(RC522_RST_PORT, &GPIO_InitStruct);
    }
    return;
}

/*
功能:
    1.RC522 Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
void Rc522Init(void)
{
    Rc522SpiInit();
    Rc522GpioInit();
    Rc522RegInit();
    return;
}

/*********************Rc522 Reg Read/Write***********************************************/
/*
功能:
    1.Write RC522 Register
参数:   
    1.addr:     要写的Register的地址
    2.ucData:   要写入Register的数据
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522WriteReg(uint8 const addr, uint8 ucData)
{
    uint8 aucTmp[2] = {0};
    uint8 i = 0;

    aucTmp[i++] = RC522_GET_ADDR_BYTE(RC522_WRITE_FLG, addr);
    aucTmp[i++] = ucData;
    SpiDataWrite(RC522_SPI_CH, aucTmp, 2);

    return;
}

/*
功能:
    1.Read RC522 Register
参数:   
    1.addr:     要读取的Register的地址
返回值: 
    读取的Register的数据
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint8 Rc522ReadReg(uint8 const addr)
{
    uint8 ucTmp = 0;
    uint8 ucReg = 0;

    ucTmp = RC522_GET_ADDR_BYTE(RC522_READ_FLG, addr);
    SpiDataRead(RC522_SPI_CH, &ucReg, 1, &ucTmp, sizeof(ucTmp));

    return ucReg;
}

/*
功能:
    判断CMD是否能自动结束
参数:
    ucCmd: 命令
返回值:
    TRUE:  能自动结束
    FALSE: 不能自动结束
输入:
    1.astCmdAttr:   存储相应命令是否自动结束的结构体
输出:void
备注:
*/
static bool Rc522ChckCmdAutoEnd(uint8 const ucCmd)
{
    uint8 i = 0;
    bool res = FALSE;
    
    for(i = 0; i < SizeOfArray(astCmdAttr); i++)
    {
        if (ucCmd == astCmdAttr[i].enCmd)
        {
            res = astCmdAttr[i].bIsAutoEnd;
            break;
        }
    }
    return res;
}

/*
功能:
    1. 清空RegComReq和RegDivReq两个中断请求标志
    2. 向RC522发送CMD
    3. 如果该命令能自动完成，则等待RegComReq中的IdleIRq标志
       如果该命令是MFAuthent则等待Status2Reg register中的MFCrypto1On
       如果相应标志等待超时，则返回FALSE
    4. 如果
参数:
    ucCmd: 命令
返回值:
    TRUE:  Success
    FALSE: 等待应答超时/错误
输入:void
输出:void
备注:
*/
static bool Rc522SendCmd(uint8 const ucCmd)
{
    bool res = FALSE;

    Rc522IrqClr();
    Rc522WriteReg(RegCmd, CmdIdle);
    if(CmdMFA == ucCmd)
    {//清空RegStatus2寄存器
        Rc522WriteReg(RegStatus2, 0x00);
    }
    Rc522WriteReg(RegCmd, ucCmd);
    if (Rc522ChckCmdAutoEnd(ucCmd & CmdMsk))
    {
        res = !Rc522WaitFlg(FlgIdle);
    }
    else if(CmdMFA == ucCmd)
    {
        res = !Rc522WaitFlg(FlgMFCrypto1);
    }
    else
    {
        res = TRUE;
    }
    if (FALSE == res)
    {
        printf("Rc522 Send Cmd Error:%x\r\n", ucCmd);
    }
    return res;
}

/*
功能:
    1.获取接收的数据中最后一个字节的有效位数
参数:   void
返回值:
    接收的数据中最后一个字节的有效位数(1-8)
输入:   void
输出:   void
备注:   void
*/
static uint8 Rc522GetRxBitsCnt(void)
{
    uint8 ucTmp = RxBitsMask & Rc522ReadReg(RegControl);
    
    if(0 == ucTmp)
    {
        ucTmp = 8;
    }
    return ucTmp;
}

/************************Rc522 Reg Cfg**************************************************/
/*
功能:
    1.Rc522 RF module On/Off
参数:   
    1.OnOff: on/off命令
返回值: void
输入:   void
输出:   void
备注:   void
*/
void Rc522RfSw(enOnOffDef OnOff)
{
    bool bIsOff = (enOff == OnOff);
    uint8  ucReg = 0;
    uint8 ucCmd = 0;
	
    {//Rceive On/Off
        ucCmd = CmdNoCmd | (bIsOff ? RcvOff : 0);
        Rc522SendCmd(ucCmd);
    }
    {//Tran On/Off
        ucReg = (bIsOff ? 0 : (Tx2RFEn | Tx1RFEn | InvTx2RFOn));
        Rc522WriteReg(RegTxCtrl, ucReg);
    }
    DelayUs(1000);
    return;
}

/*
功能:
    1.Soft power-down mode entered/exit
参数:   
    1.OnOff: on/off命令
返回值: void
输入:   void
输出:   void
备注:   void
*/
static void Rc522PwrSw(enOnOffDef PwrOnOff)
{
    uint8 ucCmd   = CmdNoCmd | ( (enOff == PwrOnOff) ? PwrDown : 0);
    uint8 ucState = 0;

    Rc522SendCmd(ucCmd);
    if (enOn == PwrOnOff)
    {
        do
        {
            ucState = Rc522ReadReg(RegCmd);
            ucState &= PwrDown;
        }
        while(ucState);
    }
    return;
}

/*
功能:
    1.数据发送配置
参数:   
    1.bIsCrc TRUE:数据包中包含CRC,FALSE:数据包中不包含CRC
返回值: void
输入:   void
输出:   void
备注:   void
*/
void Rc522TxCfg(bool bIsCrc)
{
    uint8 ucTmp = 0;
    
    if(bIsCrc)
    {/*106 kBd   Tx CRC Enable */
        ucTmp = TxSpeed106k | TxCRCEn;
    }
    else
    {/*106 kBd   Tx CRC Disable */
        ucTmp = TxSpeed106k;
    }
    Rc522WriteReg(RegTxMode, ucTmp);
    return;
}

/*
功能:
    1.数据接收配置
参数:   
    1.bIsCrc  接收数据包中是否包含CRC
    2.bIsPrty 接收数据包中是否包含校验位
返回值: void
输入:   void
输出:   void
备注:   void
*/
void Rc522RxCfg(bool bIsCrc, bool bIsPrty)
{
    uint8 ucTmp = 0;
    
    if(bIsCrc)
    {/*106 kBd   Rx CRC Enable */

        ucTmp = RxSpeed106k | RxCRCEn | RxNoErr;
    }
    else
    {/*106 kBd   Rx CRC Disable */
        ucTmp = RxSpeed106k | RxNoErr;
    }
    Rc522WriteReg(RegRxMode, ucTmp);
    
    if(bIsPrty)
    {/*Parity Enable */
        ucTmp = 0;
    }
    else
    {/*Parity Disable */
        ucTmp = ParityDisable;
    }
    Rc522WriteReg(RegMfRx, ucTmp);
    return;
}
/*********************Rc522 CMD**********************************************************/

/*
功能:
    1.启动射频数据发送并等待发送完成
参数:   
    1.TranLen     要发送的最后一个字节数据的bits长度(0-7,TX_WHOLE_BYTE表示整个字节全部发送)
返回值: void
输入:   void
输出:   void
备注:   void
*/
void Rc522StartTransCv(uint8 RxPos, uint8 TranLen)
{
    uint8 ucTmp = 0;
#if S50DBG
    static uint8 ucLst = 0;
#endif

    Rc522SendCmd(CmdTransCv);
    ucTmp = (RxPos << 4) | (TranLen & TxLastBits);
#if S50DBG
    ucLst = Rc522ReadReg(RegBitFraming);
    if(ucLst != ucTmp)
    {
        Rc522WriteReg(RegBitFraming, ucTmp);//FULL BYTE transmission/reception
    }
#endif
    Rc522WriteReg(RegBitFraming, ucTmp | StartSend);//FULL BYTE transmission/reception
    return;
}

/*
功能:
    1.结束RC522 Tranceive 指令
参数:   
    1.TranLen     要发送的最后一个字节数据的bits长度(0-7,TX_WHOLE_BYTE表示整个字节全部发送)
返回值: void
输入:   void
输出:   void
备注:   void
*/
void Rc522StopTransCv(void)
{
    Rc522SendCmd(CmdIdle);
    Rc522WriteReg(RegBitFraming, 0);//FULL BYTE transmission/reception
    return;
}

/*****************************************FIFO*******************************/

/*
功能:
    1.获取FIFO中数据长度
参数:   void
返回值: 
    FIFO中数据长度
备注:   void
注意:   void
*/
uint8 Rc522GetFifoLevel(void)
{
    uint8 ucTmp = 0;
    
    ucTmp = Rc522ReadReg(RegFIFOLevel) & FIFO_SzMsk;
    return ucTmp;
}

/*
功能:
    1.读取RC522中的数据并写入pData缓存
    2.清空FIFO
参数:
    1.pData: 指向读取缓存的指针
    2.ucLen: 要读取的数据长度
返回值: 
    1.TRUE:  Success
    2.FALSE: Fail
输入:   void
输出:   void
备注:
    1.如果FIFO中的数据<ucLen，返回错误
    2.如果FIFO中的数据>=ucLen，则读取ucLen个Bytes的数据
注意:   void
*/
bool Rc522ReadFIFO(uint8 *pData, uint8 ucLen)
{
    uint8 aucSend = RC522_GET_ADDR_BYTE(RC522_READ_FLG, RegFIFOData);
    uint8 ucDataLen = 0;//FIFO中的有效数据长度
    bool  bRes = FALSE;

    ucDataLen = Rc522GetFifoLevel();
    if (ucDataLen < ucLen)
    {//长度错误
        goto ErrReturn;
    }
    if (ucDataLen > 0)
    {
        bRes = TRUE;
        SpiDataRead(RC522_SPI_CH, pData, ucLen, &aucSend, 0);
    }
    Rc522ClrFIFO();
ErrReturn:
    if(FALSE == bRes)
    {
        printf("Rc522ReadFIFO Err!\r\n");
    }
    return bRes;
}

/*
功能:
    1.清空FIFO
    2.写入指定长度的数据到FIFO
参数:
    pData:  指向写入缓存的指针
    uclen:  待写入的数据长度
返回值: void
输入:   void
输出:   void
备注:
    1.发送/接收数据均包含CRC和parity 
注意:   void
*/
void Rc522WriteFIFO(uint8 *pData, uint8 uclen)
{
    uint8 *pucTmp = NULL;
    uint16 usIndex = 0;
    uint8  i =0;

    pucTmp = MemMalloc(enMemSmall, &usIndex);
    if(pucTmp)
    {
        Rc522ClrFIFO();
        pucTmp[i++] = RC522_GET_ADDR_BYTE(RC522_WRITE_FLG, RegFIFOData);
        memcpy(&pucTmp[i], pData, uclen);
        i += uclen;
        SpiDataWrite(RC522_SPI_CH, pucTmp, i);
        MemFree(enMemSmall, usIndex);
    }
    else
    {
        PrintErr(Rc522WriteFIFO, 0, 0);
    }
    
    return;
}

/*
功能:
    1.清空internal buffer
参数:   void
返回值: void
备注:   void
注意:   void
*/
static void Rc522ClrIntrBuffer(void)
{
    uint8 aucTmp[25] = {0};

    Rc522WriteFIFO(aucTmp, sizeof(aucTmp));
    Rc522SendCmd(CmdMem);    
	Rc522ClrFIFO();
    return;
}

/*
功能:
    1.用Rc522产生10个随机数(Byte)并写入缓存
参数:   pucRead
返回值: 
    1.TRUE:  Success
    2.FALSE: Fail
输入:   void
输出:   
    1.pucRead   指向用于缓存数据的指针
备注:   无
注意:   
*/
bool Rc522GetRanData(uint8 *pucRead)
{
    bool bRes = FALSE;

    Rc522SendCmd(CmdGRID);
    Rc522ClrFIFO();
    Rc522SendCmd(CmdMem);
    bRes = Rc522ReadFIFO(pucRead, RANDOM_SZ);
    return bRes;
}
#if RC522_DBG

/*
功能:
    1.串口打印FIFO中的数据
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522PrintFIFO(void)
{
    uint8 *pucTmp = NULL;
    uint16 usIndex = 0;
    uint8  ucCnt   = 0;//用于成功读取的存储fifo中的数据长度

    pucTmp = (uint8 *)MemMalloc(enMemSmall64, &usIndex);
    if(pucTmp)
    {
        Rc522ReadFIFO(pucTmp, RC522_BUFFER_SZ);
        UartPrintBuffer(pucTmp, ucCnt);
        MemFree(enMemSmall, usIndex);
    }


    return;
}

/*
功能:
    1.向FIFO中写入一个0(测试用)
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522WriteFIFOZero(void)
{
    uint8 ucTmp = 0;

    Rc522WriteFIFO(&ucTmp, sizeof(ucTmp));
    return;
}

/*
功能:
    1.测试RC522与MCU通信是否正常
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void Rc522SelfTest(void)
{
    Rc522Rst();//硬件复位
    Rc522IrqClr();
    Rc522ClrFIFO();
    Rc522SendCmd(CmdSoftRst);//soft rest
    Rc522ClrIntrBuffer();/*清空internal buffer */
    Rc522WriteReg(AutoTestReg, 0x09);//Enable the self test 
    Rc522WriteFIFOZero();//Write 00h to the FIFO buffer.
    Rc522SendCmd(CmdCRC);//Start the self test with the CalcCRC command.
    while(0 == (Rc522ReadReg(RegStatus1) & CRCReady));
    Rc522PrintFIFO();

    return;
}

#endif
/**************************Timer***************************/

/*
功能:
    1.配置RC522 Timer寄存器以等待一段特定时间
    2.清TimerIRq Bit
    3.如果bIsAuto为TRUE,则将Timer设置为自启动模式(发送完成后启动),否则，
      设置则启动Timer
参数:   
    1.usTime:   以ms为单位的时间
    2.bIsAuto:  是否在发送完成后自动启动Timer
返回值: void
输入:   void
输出:   void
备注:   
    
注意:   void
*/
void Rc522TimerCfg(uint16 usTime, bool bIsAuto)
{
    uint16 usTReload = Rc522GetTReload(usTime);

    Rc522WriteReg(RegTReloadH,   (usTReload >> 8));
    Rc522WriteReg(RegTReloadL,   (usTReload));
    if (bIsAuto)
    {
        uint8 ucTmp = 0;

        ucTmp = Rc522ReadReg(RegTMode);
        Rc522WriteReg(RegTMode,   (usTReload));
    }
    else
    {
        Rc522StartT();
    }
    return;
}

/*
功能:
    1.将CMD code(0x60 KeyA, 1Byte) + BlockAddr(1Byte) + KeyA(6Byte) + UID(4Byte)写入FIFO,
      并执行MFRC522 MFAuthent 命令 并返回执行结果
参数:   
    1.ucCmd:  Authentication command code (60h, 61h)
    2.ucBlockAddr: Block address
    3.pucKeyA: 指向KeyA的指针

    4.pucUid : 指向Uid的
返回值: 
    1.TRUE:  认证成功
    2.FALSE: 认证失败
输入:   void
输出:   void
备注:   无
注意:   
*/
bool Rc522MFAuthent(uint8 ucCmd, uint8 ucBlockAddr, uint8 *pucKeyA, uint8 *pucUid)
{
    uint8 i = 0;
    uint8 aucTmp[12] = {0};
    bool  bRes = FALSE;

    aucTmp[i++] = ucCmd;
    aucTmp[i++] = ucBlockAddr;
    memcpy(&aucTmp[i], pucKeyA, 6);
    i += 6;
    memcpy(&aucTmp[i], pucUid, 4);
    i += 4;
    Rc522WriteFIFO(aucTmp, sizeof(aucTmp));

    bRes = Rc522SendCmd(CmdMFA);
    return bRes;
}

#ifdef __cplusplus
}
#endif
#endif

