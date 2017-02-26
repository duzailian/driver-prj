#ifndef __SDIO_C
#define __SDIO_C

#include "sdcmdarg.h"
#include "sdreg.h"
#include "sdstate.h"
#include "sd_cmd.h"


typedef enum {
    SD_IdMode,//Card Identification Mode
    SD_DataMode,//Data Transfer Mode
}enSdioSpeedModeDef;
static enSdioSpeedModeDef enCurSpeedMode = SD_IdMode;

/*ID mode & DATA mode CLK DIV */
/*SDIO_CK frequency = HCLK / [CLKDIV + 2].*/
#define SD_IDMODE_CLK_DIV          200//ID mode 
#define SD_DATAMODE_CLK_DIV        2//data mode

/*当前的ck频率(KHZ)*/
#define SD_CK_FREQ                  (72000 \
                                    / (((enCurSpeedMode == SD_IdMode) ? SD_IDMODE_CLK_DIV : SD_DATAMODE_CLK_DIV) + 2))

#define SD_DTIMER_TIME            SD_CK_FREQ//每增加1ms SDIO_DTIMER寄存器所需的增量

#define SDIO_ClockEnable                     ((uint32_t)0x000000100)
#define SDIO_Response_Mask                   ((uint32_t)0x000000C0)

#define TIMEOUT_CNT                100
#define POWER_ON_RETRY             100

#define SDIO_CERR_MASK            (SDIO_FLAG_CCRCFAIL | SDIO_FLAG_CTIMEOUT | SDIO_FLAG_TXUNDERR\
                                                               | SDIO_FLAG_RXOVERR)
#define SDIO_DERR_MASK            (SDIO_FLAG_DCRCFAIL | SDIO_FLAG_DTIMEOUT | SDIO_FLAG_TXUNDERR \
                                                               | SDIO_FLAG_RXOVERR    | SDIO_FLAG_STBITERR)
#define SDIO_CERR_CLR             (SDIO_IT_CMDSENT | SDIO_IT_CMDREND | SDIO_IT_CCRCFAIL | SDIO_IT_CTIMEOUT  \
                                                                | SDIO_IT_TXUNDERR | SDIO_IT_RXOVERR  | SDIO_IT_STBITERR)
#define SDIO_DERR_CLR             (SDIO_IT_DATAEND | SDIO_IT_DTIMEOUT | SDIO_IT_DCRCFAIL   \
                                                                | SDIO_IT_TXUNDERR | SDIO_IT_RXOVERR   | SDIO_IT_STBITERR)
/*操作执行中*/
#define SDIO_STA_ACT							(SDIO_FLAG_TXACT | SDIO_FLAG_RXACT | SDIO_FLAG_CMDACT)
/*GPIO pin*/
/*DATA PIN*/
#define SDIO_D0_PIN     GPIO_Pin_8
#define SDIO_D1_PIN     GPIO_Pin_9
#define SDIO_D2_PIN     GPIO_Pin_10
#define SDIO_D3_PIN     GPIO_Pin_11
/*DATA PORT*/
#define SDIO_DAT_PORT   GPIOC

/*CK PIN*/
#define SDIO_CK_PIN     GPIO_Pin_12
/*CK PORT*/
#define SDIO_CK_PORT    GPIOC

/*CMD PORT*/
#define SDIO_CMD_PIN    GPIO_Pin_2
/*CMD PORT*/
#define SDIO_CMD_PORT   GPIOD

/*进入/退出 identification mode */
#define SD_ENTER_ID_MODE()                      do{SDIO->CLKCR &= ~0xff;SDIO->CLKCR |= SD_IDMODE_CLK_DIV;  enCurSpeedMode = SD_IdMode;}while(0)
#define SD_EXIT_ID_MODE()                       do{SDIO->CLKCR &= ~0xff;SDIO->CLKCR |= SD_DATAMODE_CLK_DIV; enCurSpeedMode = SD_DataMode;}while(0)

#define SD_ChckBusy()                           while(0 == (SDIO_DAT_PORT->IDR & SDIO_D0_PIN))
	
#define SD_MAX_SECTOR          (unCsdBuf.CSD.C_SIZE * 1024)

#define BIT32_SZ                (32 / 8)
typedef enum {
    SD_HCCard,//High Capacity SD Memory Card
    SD_SCCard,//Standard Capacity SD Memory Card
}enSdTypeDef;

typedef enum{
    SdBlock,
    SdStream,
}enSdDtModeDef;

/*card state*/
typedef enum {
    SdCdStaIdle,//idle
    SdCdStaReady,//ready
    SdCdStaIdent,//SdCdSta
    SdCdStaStby,
    SdCdStaTran,
    SdCdStaData,
    SdCdStaRcv,
    SdCdStaPrg,
    SdCdStaDis,
}enSdCdStaDef;

/*earse cmd type*/
typedef enum {
    SdEarseStart,//earse start addr set
    SdEarseStop,//earse stop addr set
    SdEarseAct,//执行earse
}enSdEarseCmdDef;


typedef struct {
    uint8  ucCmd;
    uint16 usErrCode;
}stCmdErrFilterDef;

typedef struct {
    uint32_t  ulArg;
    uint32_t  ulCmd;
}stCmdSendDef;

typedef uint16 (*SdInitFuncDef)(void);

static bool _SdChckR1(uint32 ulStateMsk);
static uint8 _SdChckR7(void);
static uint16 _SdSoftRest(void);
static uint16 _SdHC_Chck(void);
static uint16 _SdPowerOn(void);
static uint16 _SdAskCID(void);
static uint16 _SdAskRCA(void);
static uint16 _SdAskCSD(void);
static uint16 _SdSetDataBus(void);
static uint16 _SdSelectCard(void);
static uint16 _SdAskCardSta(void);
static uint16 _SdSwitchFunc(void);
static uint16 _SdAskSdSta(void);
static uint16 _SdStopTran(void);


static enSdTypeDef SdCardType = SD_SCCard;//
const SdInitFuncDef SdInitFunc[] = {_SdSoftRest, _SdHC_Chck, _SdPowerOn, _SdAskCID, _SdAskRCA,
                                    _SdAskCSD, _SdSelectCard, _SdAskCardSta, _SdSetDataBus, _SdAskSdSta, };////dAskCardSta, };//_SdAskSdSta};//_SdSwitchFunc};
//static uint8  aucCsdBuf[RCA_SZ];

static uint16  usRCA;// relative card address 
static enSdCdStaDef enCurCdSta = SdCdStaIdle;//Physical Layer Simplified Specification V2.0 中的Figure 4-1和Figure 4-3:

#if SD_DBG
static  __align(4) uint8 aucSdReadBuffer[SD_DATA_BLOCK_SZ] = {0};
static  __align(4) uint8 aucSdWriteBuffer[SD_DATA_BLOCK_SZ] = "duzailian ";
#endif
static unSdStaDef unSdSta = {0};
uint8 ucSdStaFlg = SD_NOINIT | SD_PROTECT;
uint32 ulSdMaxSector = 0;

static const stCmdErrFilterDef aucResErrFilter[] = {//用于过滤Sdio片内外设与协议不匹配时的错误
    {SD_CMD_SD_SEND_OP_COND, SDIO_FLAG_CCRCFAIL},
    {SD_CMD_ALL_SEND_CID,    0},
    {SD_CMD_SEND_CSD,    0},
};

/*
功能:
    Sdio相关Rcc初始化
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void SdioRccInit(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_SDIO | RCC_AHBPeriph_DMA2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
}

/*
功能:
    Sdio相关Gpio初始化
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void SdioGpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.GPIO_Pin   = SDIO_D0_PIN | SDIO_D1_PIN | SDIO_D2_PIN | SDIO_D3_PIN | SDIO_CK_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SDIO_DAT_PORT, &GPIO_InitStruct);
    GPIO_SetBits(SDIO_DAT_PORT, SDIO_D0_PIN | SDIO_D1_PIN | SDIO_D2_PIN | SDIO_D3_PIN);

    GPIO_InitStruct.GPIO_Pin   = SDIO_CMD_PIN;
    GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SDIO_CMD_PORT, &GPIO_InitStruct);
    return;
}

/*
功能:
    Stm32 sdio外设寄存器初始化
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void SdioRegInit(void)
{
/*CLK_Init*/
    SDIO->CLKCR = SDIO_BusWide_4b | SDIO_ClockEdge_Rising | SDIO_ClockBypass_Disable 
                  | SDIO_ClockPowerSave_Disable | SDIO_HardwareFlowControl_Enable
                  | SD_IDMODE_CLK_DIV | SDIO_ClockEnable;
    SDIO->POWER = SDIO_PowerState_ON;
    DelayMs(10);
    return;
}

/*
功能:
    过滤掉因为协议中某些SD卡对某些命令的应答数据没有Crc导致STM32 SDIO外设寄存器误报的Error Code
参数:   
    1.ulCmd: 通过SDIO发送的命令
    2.err:   STM32 SDIO外设寄存器返回的Error Code
返回值: 
    1.过滤后的Error Code
输入:   void
输出:   void
备注:   void
注意:
    1.aucResErrFilter为存储Error Code滤波参数的数组
        其中，ucCmd为需要滤掉的Error Code对应的cmd
        usErrCode为需要滤掉的Error Code
*/
uint16 SdioCRecvCmdChck(const uint32_t ulCmd, uint16 err)
{
    uint8 i = 0;
    uint16 usTmp = (ulCmd != SDIO->RESPCMD);

    if (usTmp)
    {
        for (i = 0; i < SizeOfArray(aucResErrFilter); i++)
        {
            if(ulCmd == aucResErrFilter[i].ucCmd)
            {
                err &= ~(aucResErrFilter[i].usErrCode);
                break;
            }
        }
    }

    return err;
}

/*
功能:
    完成SDIO发送数据前，STM32 SDIO外设寄存器的配置
参数:   
    1.ulDataLen: 要发送的数据长度
    2.rw:   数据传输方向: enRead/从卡中读取数据  enWrite/写入数据到卡中
    3.DtMode: 数据传输模式: SdBlock/块模式， SdStream/流模式
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void SdioDataRegCfg(uint32 ulDataLen, enDataDirDef rw, enSdDtModeDef DtMode)
{
    bool bIsRead = (rw == enRead);
    bool bIsBlock = (DtMode == SdBlock);

    SDIO->ICR       = SDIO_DERR_CLR;
    SDIO->DTIMER    = SD_DTIMER_TIME * 110;//110ms
    SDIO->DLEN      = ulDataLen;
    SDIO->DCTRL     = SDIO_DPSM_Enable | (bIsRead ? SDIO_TransferDir_ToSDIO : SDIO_TransferDir_ToCard) | (bIsBlock ? SDIO_TransferMode_Block : SDIO_TransferMode_Stream)
                      | SDIO_DmaEN | SDIO_DataBlockSize_512b;
    return;
}

/*
功能:
    向SD卡发送/接收数据
参数:   
    1.rw: 数据传输方向: enRead/从卡中读取数据  enWrite/写入数据到卡中
    2.pucData: 指向收/发缓存的指针
    3.usLen: 数据传输长度
返回值: 
    1.Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
uint16 SdioRwData(enDataDirDef rw, uint8 *pucData, uint16 usLen)
{
    uint16 err    = 0;
    uint8  ucStep = 0;
    
    if (rw == enRead)
    {
        while(0 == (SDIO_FLAG_RXDAVL & SDIO->STA))
        {
            err = SDIO->STA & SDIO_DERR_MASK;
            if(err)
            {
                ucStep = 0;
                goto ErrReTurn;
            }
        }
    }
    DmaSdioCfg(rw, pucData, usLen);
    err = DmaSdioWaitCmpt();
    if (err)
    {
        ucStep = 1;
        goto ErrReTurn;
    }
    return 0;
ErrReTurn:
    PrintErr(SdioRwData, ucStep, err);
    return err;
}

/*
功能:
    通过SDIO发送cmd及参数
参数:   
    1.SDIO_CmdStruct为指向包含cmd及参数的结构体指针
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
static void _SDIO_SendCommand(stCmdSendDef *SDIO_CmdStruct)
{
    SDIO->ARG = SDIO_CmdStruct->ulArg;
    SDIO->CMD = SDIO_CmdStruct->ulCmd;
    return;
}

/*
功能:
    1.等待SDIO的STA寄存器中与ChckMsk对应的位置位
    2.如果期间STA寄存器有错误FLG,则返回Error Code
参数:   
    1.ChckMsk:需要等待置位的Bit
(SDIO_FLAG_CMDSENT etc.)
返回值: 
    1.0:      Success
    2.Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 SdioCmdSendChck(uint32_t ChckMsk)
{
    uint16 err          = 0 ;
    uint16 TimeOutCnt = TIMEOUT_CNT;

    while(SDIO->STA & SDIO_FLAG_CMDACT);//等待命令发送完成
    while(--TimeOutCnt)
    {
        err = SDIO->STA & SDIO_CERR_MASK;
        if (err)
        {
            goto ErrReTurn;
        }
        if (SDIO->STA & ChckMsk)
        {//命令发送完成
            goto success;
        }
    	DelayUs(10);
    }
    err = SDIO_FLAG_CTIMEOUT;
ErrReTurn:
    SDIO->ICR = SDIO_CERR_CLR;
    return err;
success:
    return 0;
}

/*
功能:
    1.等待没有应答的SDIO命令发送完成
    2.如果期间STA寄存器有错误FLG,则返回Error Code
参数:   void
返回值: 
    1.0:      Success
    2.Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 SdioCSendNoResErrChck(void)
{
    return SdioCmdSendChck(SDIO_FLAG_CMDSENT);
}

/*
功能:
    1.等待有应答的SDIO命令发送完成
    2.如果期间STA寄存器有错误FLG,则返回Error Code
参数:   
    1.
返回值: 
    1.0:      Success
    2.Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   
    1.由于部分SD卡对部分SDIO命令的应答不包含Crc
*/
static uint16 SdioCSendResErrChck(stCmdSendDef *SDIO_CmdStruct)
{
    uint16 err = 0;
    
    err = SdioCmdSendChck(SDIO_FLAG_CMDREND);
    if (err)
    {
        err = SdioCRecvCmdChck(SDIO_CmdStruct->ulCmd & SD_CMD_MASK, err);
    }
    return err;
}

/*
功能:
    通过SDIO向SD卡发送cmd及参数
参数:   
    1.SDIO_CmdInitStruct 发送CMD的结构体
        SDIO_CmdStruct->ulCmd:写入SDIO_CLKCR寄存器的值
        SDIO_CmdStruct->ulArg:写入SDIO_ARG  寄存器的值
返回值: 
    0:      Success
    Others: STA寄存器的 Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 SdioSendRegularCmd(stCmdSendDef *SDIO_CmdStruct)
{
    _SDIO_SendCommand(SDIO_CmdStruct);
    return ((SDIO_CmdStruct->ulCmd & SDIO_Response_Mask) == SDIO_Response_No) ? SdioCSendNoResErrChck() : SdioCSendResErrChck(SDIO_CmdStruct);
}

/*
功能:
    通过SDIO发送acmd及参数
参数:   
    1.SDIO_CmdInitStruct 发送CMD的结构体
        SDIO_CmdStruct->ulCmd:写入SDIO_CLKCR寄存器的值
        SDIO_CmdStruct->ulArg:写入SDIO_ARG  寄存器的值
返回值: 
    0:      Success
    Others: STA寄存器的 Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 SdSendAppCmd(stCmdSendDef *SDIO_CmdStruct)
{
    stCmdSendDef stTmp = {0};
    uint16      err = 0; 
    uint8       ucStep = 0;

    stTmp.ulArg  = (uint32)usRCA << 16;
    stTmp.ulCmd  = SD_CMD_APP_CMD | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdioSendRegularCmd(&stTmp);
    if (err)
    {
        ucStep = 1;
        goto ErrReTurn;
    }
    if (FALSE == _SdChckR1(SD_STATE_APP_CMD))
    {
        err = 0xff;
        ucStep = 2;
        goto ErrReTurn;
    }
     
    err = SdioSendRegularCmd(SDIO_CmdStruct);
    if (err)
    {
        ucStep = 3;
        goto ErrReTurn;
    }
    return 0;
ErrReTurn:
    PrintErr(SdSendAppCmd, ucStep, err);
    return err;
}

/*
功能:
    1.读取STM32的SDIO_RESPx寄存器(数据应答寄存器),获取应答数据,并按R2格式解析出CID或CSD
参数:   
    1.enSdR2TypeDef 获取的数据类型
        SD_CID:获取CID
        SD_CSD:获取CSD
返回值: void
输入:   void
输出:   
    1.unCidBuf 存储CID的缓存
    2.unCsdBuf 存储CSD的缓存
备注:   void
注意:   void
*/
static void _SdChckR2(enSdR2TypeDef enParType)
{
    uint32 aultmp[SD_RES_LONG_SZ / 32] = {0};
    uint8 const *puctmp = (uint8 *)&aultmp;

    aultmp[0] = SDIO->RESP4;
    aultmp[1] = SDIO->RESP3;
    aultmp[2] = SDIO->RESP2;
    aultmp[3] = SDIO->RESP1;

    if (SD_CID == enParType)
    {
        puctmp++;
        memcpy(&unCidBuf.DATA, puctmp, sizeof(unCidBuf.DATA));
    }
    else
    {
        memcpy(&unCsdBuf.DATA, puctmp, sizeof(unCsdBuf.DATA));
        if ((0 == unCsdBuf.CSD.PERM_WRITE_PROTECT) && (0 == unCsdBuf.CSD.TMP_WRITE_PROTECT))
        {//未写保护
            ucSdStaFlg &= ~SD_PROTECT;
        }
        ulSdMaxSector = SD_MAX_SECTOR;
    }
    return;
}

/*
功能:
    1.读取STM32的SDIO_RESPx寄存器,获取应答数据,并按R7格式解析
    2.判断check pattern和voltage accepted是否符合要求
参数:   void
返回值: 
    0:      OK
    Others: ErrorCode
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint8 _SdChckR7(void)
{
    uint32 ulTmp = 0;
    uint8 err = 0;
    
    ulTmp = SDIO->RESP1;
    if (((ulTmp & 0xff) !=  SD_CMD_ARG_PATTERN)
        || (((ulTmp >> 8) & 0x0f) !=  SD_CMD_ARG_VHS))
    {
        err = 1;
    }

    return err;
}

/*
功能:
    1.读取STM32的SDIO_RESPx寄存器,获取应答数据,并按R3格式解析
    2.判断Card power up是否完成
参数:   void
返回值: 
    TRUE:  Card power up complete
    FALSE: Card power up not complete
输入:   void
输出:   void
备注:   void
注意:   void
*/
static bool _SdChckR3(void)
{
    uint32 ulTmp = 0;

    ulTmp = SDIO->RESP1;
    ulTmp >>= 31;

    return ulTmp ? TRUE : FALSE;
}

/*
功能:
    1.读取STM32的SDIO_RESPx寄存器,获取应答数据,并按R3格式解析
    2.判断Card power up是否完成
参数:   void
返回值: 
    TRUE:  Card power up complete
    FALSE: Card power up not complete
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 _SdChckR6(void)
{
    uint32 ulTmp = 0;
    uint16 err = 0;
    
    ulTmp = SDIO->RESP1;

    err = ulTmp & (0xe000);//err chck
    if (0 == err)
    {
        usRCA = ulTmp >>= 16;
    }
    return err;
}

/*
功能:
    1.读取STM32的SDIO_RESPx寄存器,获取应答数据,并按R1格式解析
    2.判断ulStateMsk对应的Card Status Bit是否置位
    3.获取当前的CURRENT_STATE 并存入enCurCdSta
参数:   
    1.ulStateMsk: 存储要获取的Card Status Bit
返回值: 
    TRUE:  ulStateMsk对应的Card Status Bit为1
    FALSE: ulStateMsk对应的Card Status Bit为0
输入:   void
输出:   
    1.enCurCdSta当前SD状态
备注:   void
注意:    
    1.If the command execution causes a state change, it will be visible to the host in the 
      response to the next command. 
*/
static bool _SdChckR1(uint32 ulStateMsk)
{
    uint32 ulTmp = 0;
    bool   res = FALSE;
    
    ulTmp = SDIO->RESP1;
    if (ulTmp & ulStateMsk)
    {
        res = TRUE;
    }
    enCurCdSta = (enSdCdStaDef)((ulTmp >> 9) & 0xff);

    return res;
}

/*
功能:
    1.根据SdCardType确定acmd41的Argument,发送acmd41并等待发送完成
    2.等待power up完成
参数:   
    1.ulStateMsk: 存储要获取的Card Status Bit
返回值: 
    TRUE:  ulStateMsk对应的Card Status Bit为1
    FALSE: ulStateMsk对应的Card Status Bit为0
输入:
    1.SdCardType:
        SD_HCCard //High Capacity SD Memory Card
        SD_SCCard //Standard Capacity SD Memory Card
输出:   void
备注:   
    1.SD_SEND_OP_COND (ACMD41) is used to start initialization and to check if the card has completed initialization
注意:   void
*/
static uint16 _SdPowerOn(void)
{
    bool   bPwrSta      = FALSE;
    uint16 usRetryCnt   = POWER_ON_RETRY;
    uint16 err = 0; 
    uint8  ucStep = 0;
    stCmdSendDef stCmdSend = {0};

    stCmdSend.ulCmd  = SD_CMD_SD_SEND_OP_COND | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    stCmdSend.ulArg  = ((SD_HCCard == SdCardType) ? SD_CMD_ARG_HCS : 0) | SD_CMD_ARG_VDD;

    while(!bPwrSta && usRetryCnt)
    {
        err = SdSendAppCmd(&stCmdSend);
        if (err)
        {
            ucStep = 0;
            goto ErrReTurn;
        }
        bPwrSta = _SdChckR3();
        usRetryCnt--;
    }

    if (0 == bPwrSta)
    {//fail
        ucStep = 1;
        err = 0xff;
        goto ErrReTurn;
    }        
    return 0;
ErrReTurn:
    PrintErr(_SdPowerOn, ucStep, err);
    return err;
}

/*
功能:
    1.Soft Rest
参数:   void
返回值: 
    1.0:      Success
    2.Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 _SdSoftRest(void)
{
    uint16   err = 0;
    stCmdSendDef stCmdSend = {0};
    
    /*cmd0*/
    stCmdSend.ulArg  = SD_CMD_ARG_NULL;
    stCmdSend.ulCmd  = SD_CMD_GO_IDLE_STATE | SDIO_CPSM_Enable | SDIO_Response_No | SDIO_Wait_No;
    err = SdioSendRegularCmd(&stCmdSend);
    if (err)
    {
        PrintErr(_SdSoftRest, 0, err);
    }
    return err;
}

/*
功能:
    1.发送CMD13并接收SD的应答
    2.将接收自SD卡的数据按R1解析以确定SD卡状态(检查是否有错误发生)
    3.不断发送CMD13,直到SD卡Operation Complete并退出prg状态
    4.返回Error Code
参数:   void
返回值: 
    1.0:      Success
    2.Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 _SdAskCardSta(void)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    stCmdSendDef stCmdSend = {0};

    SD_ChckBusy();
start:
    /*cmd13*/
    stCmdSend.ulCmd  = SD_CMD_SEND_STATUS | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    stCmdSend.ulArg  = (uint32)usRCA << 16;
    err = SdioSendRegularCmd(&stCmdSend);
    if (err)
    {
        ucStep = 0;
        goto ErrReTurn;
    }
    if (_SdChckR1(CS_ERROR))
    {
        err = 0xff;
        ucStep = 1;
        goto ErrReTurn;
    }
    if (SdCdStaPrg == enCurCdSta)
    {
        goto start;
    }
    return 0;
ErrReTurn:
    PrintErr(_SdAskCardSta, ucStep, err);
    return err;
}

/*
功能:
    1.不断发送SD Card状态查询命令直到SD卡可以接收来自MCU的数据
    2.返回Error Code
参数:   void
返回值: 
    1.0:      Success
    2.Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 _SdWaitDataW(void)
{
    uint16   err = 0;
    uint8    ucStep = 0;

    do 
    {
        err = _SdAskCardSta();
        if (err)
        {
            ucStep = 0;
            goto ErrReTurn;
        }
    }
    while((FALSE == _SdChckR1(SD_READY_FOR_DATA)));
    return 0;
ErrReTurn:
    PrintErr(_SdWaitDataW, ucStep, err);
    return err;
}

/*
功能:
    1.发送CMD8以确定SD卡是
参数:   void
返回值: 
    1.0:      Success
    2.Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 _SdHC_Chck(void)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    stCmdSendDef stCmdSend = {0};

    /*cmd8*/
    stCmdSend.ulArg  = (SD_CMD_ARG_VHS << 8) | SD_CMD_ARG_PATTERN;//volDefe supplied & check pattern 
    stCmdSend.ulCmd  = SD_CMD_SEND_IF_COND | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdioSendRegularCmd(&stCmdSend);
    if (err & ~SDIO_FLAG_CTIMEOUT)
    {//除等待应答外的错误发生则直接返回
        ucStep = 0;
        goto ErrReTurn;
    }
    if (err & SDIO_FLAG_CTIMEOUT)
    {//Standard Capacity
        /*Ver2.00 or later SD Memory Card(voltage mismatch)
          or Ver1.X SD Memory Card
          or not SD Memory Card*/
        SdCardType = SD_SCCard;
        err         = 0;
        goto success;
    }
    else if (_SdChckR7())
    {
        err = 0xff;
        ucStep = 1;
        goto ErrReTurn;
    }
    else
    {
        SdCardType = SD_HCCard;
    }
success:
    return 0;
ErrReTurn:
    PrintErr(_SdHC_Chck, ucStep, err);
    return err;
}

/*
功能:
    1.串口打印SD卡相关信息(MID/OID/PNM/PRV/PSN/MDT)
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
void PrintSdInfo(void)
{
    printf("SD Card infor:\r\n");

    /*MID*/
    {
        printf("SD Card MID:\r\n");
        PrintInBinary(&(unCidBuf.CID.MID), sizeof(unCidBuf.CID.MID));
        printf("\r\n");
    }

    /*OID*/
    {
        printf("SD Card OID:%c%c\r\n", unCidBuf.CID.OID[0], unCidBuf.CID.OID[1]);
    }

    /*PNM*/
    {
        char acTmp[6] = {0};

        memcpy(acTmp, unCidBuf.CID.PNM, sizeof(unCidBuf.CID.PNM));
        printf("SD Card PNM:%s\r\n", acTmp);
    }

    /*PRV*/
    {
        printf("SD Card PRV:\r\n");
        PrintInBCD(unCidBuf.CID.PRV);
        printf("\r\n");
    }

    /*PSN*/
    {
        printf("SD Card PSN:\r\n");
        PrintInBinary(unCidBuf.CID.PSN, sizeof(unCidBuf.CID.PSN));
        printf("\r\n");
    }

    /*MDT*/
    {
        uint16 usTmp = unCidBuf.CID.MDT;
        
        printf("SD Card MDT:20%d,%d\r\n", usTmp >> 4, usTmp & 0x0f);
    }
    return;
}

/*
功能:
    1.request card identification
    2.返回Error Code
参数:   void
返回值:     
    0:      Success
    Others: Error Code
输入:   void
输出:   
    1.unCidBuf 存储CID的缓存
备注:   void
注意:   void
*/
static uint16 _SdAskCID(void)
{
    uint16 err = 0;
    uint8  ucStep = 0;
    stCmdSendDef stCmdSend = {0};

    stCmdSend.ulCmd  = SD_CMD_ALL_SEND_CID | SDIO_CPSM_Enable | SDIO_Response_Long | SDIO_Wait_No;
    stCmdSend.ulArg  = SD_CMD_ARG_NULL;
    err = SdioSendRegularCmd(&stCmdSend);
    if (err)
    {
        ucStep = 0;
        goto ErrReTurn;
    }
    _SdChckR2(SD_CID);
    PrintSdInfo();
    return 0;
ErrReTurn:
    PrintErr(_SdAskCID, ucStep, err);
    return err;
}

/*
功能:
    1.request relative card address 
    2.如果获取到的RCA为0，则重新向SD卡发送请求
    3.返回Error Code
参数:   void
返回值:     
    0:      Success
    Others: Error Code
输入:   void
输出:   
    1.usRCA
备注:   void
注意:   void
*/
static uint16 _SdAskRCA(void)
{
    uint16 err = 0;
    uint8  i = POWER_ON_RETRY;
    uint8  ucStep = 0;
    stCmdSendDef stCmdSend = {0};

start:
    stCmdSend.ulCmd  = SD_CMD_SEND_RCA | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    stCmdSend.ulArg  = SD_CMD_ARG_NULL;
    err = SdioSendRegularCmd(&stCmdSend);
    if (err)
    {
        ucStep = 0;
        goto ErrReTurn;
    }
    if (_SdChckR6())
    {
        err = 0xff;
        ucStep = 1;
        goto ErrReTurn;
    }
    
    if ((0 == usRCA) && i)
    {
        i--;
        goto start;
    }
    return 0;
ErrReTurn:
    PrintErr(_SdAskRCA, ucStep, err);
    return err;
}

/*
功能:
    1.Card Initialization and Identification Process 
    2.如果执行过程中有错误发生，则终止初始化及识别流程,并返回Error Code
参数:   void
返回值:     
    0:      Success
    Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   void
*/
static uint16 SdioCardInit(void)
{
    uint8       i     = 0;
    uint8       step  = 0;
    uint16      err   = 0; 

    for (i = 0; i < SizeOfArray(SdInitFunc); i++)
    {
        err = SdInitFunc[i]();
        if (err)
        {
            step = i;
            goto ErrReTurn;
        }
    }
    SD_EXIT_ID_MODE();
    return 0;
    
ErrReTurn:
    PrintErr(SdioCardInit, step, err);
    return err;
}

/*
功能:
    1.SDIO相关及SD Card Init
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
注意:   void
*/
void SdCardInit(void)
{
    SdioRccInit();
    SdioGpioInit();
    SdioRegInit();
    if (0 == SdioCardInit())
    {
        ucSdStaFlg &= ~SD_NOINIT;
    }
    return;
}

/*
功能:
    1.获取指定RCA对应的SD Card的CSD并存入unCsdBuf
参数:   void
返回值: 
    0:      Success
    Others: Error Code
输入:   
    1.usRCA:  relative card address 
输出:   
    1.unCsdBuf:存储CSD的结构体
备注:   void
注意:   void
*/
static uint16 _SdAskCSD(void)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    stCmdSendDef stCmdSend = {0};
    
    /*cmd0*/
    stCmdSend.ulCmd  = SD_CMD_SEND_CSD | SDIO_CPSM_Enable | SDIO_Response_Long | SDIO_Wait_No;
    stCmdSend.ulArg  = (uint32)usRCA << 16;
    err = SdioSendRegularCmd(&stCmdSend);
    if (err)
    {
        ucStep = 0;
        goto ErrReTurn;
    }
    
    _SdChckR2(SD_CSD);
    return 0;
ErrReTurn:
    PrintErr(_SdAskCSD, ucStep, err);
    return err;
}

/*
功能:
    1.等待SD Card非忙(DAT0 data line非0)
    2.检查SD Card State Error
参数:   void
返回值: 
    0:      Success
    Others: Error Code
输入:   void 
输出:   void
备注:   void
注意:   void
*/
static uint16 _SdChckR1b(void)
{
    uint16      err     = 1;

    SD_ChckBusy();
    err = _SdChckR1(CS_ERROR);
    
    return err;
}

/*
功能:
    1.发送CMD7,使指定的RCA对应的SD Card 进入Transfer State
    2.检查SD Card State Error并返回
参数:   void
返回值: 
    0:      Success
    Others: Error Code
输入:   
    1.usRCA relative card address 
输出:   void
备注:   void
注意:   
    1.如果调用该函数前,usRCA为0,所有收到CMD7的SD卡都将进入Stand-by State
*/
static uint16 _SdSelectCard(void)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    stCmdSendDef stCmdSend = {0};
    
    /*cmd0*/
    stCmdSend.ulCmd  = SD_CMD_SELECT_CARD | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    stCmdSend.ulArg  = (uint32)usRCA << 16;
    err = SdioSendRegularCmd(&stCmdSend);
    if (err)
    {
        ucStep = 0;
        goto ErrReTurn;
    }
    
    if (_SdChckR1b())
    {
        err = 0xff;
        ucStep = 1;
        goto ErrReTurn;
    }
    return 0;
ErrReTurn:
    PrintErr(_SdSelectCard, ucStep, err);
    return err;
}

/*
功能:
    1.设置Data Bus 宽度
参数:   void
返回值: 
    0:      Success
    Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   
    1.程序编译时Data Bus 宽度已通过SD_DATA_BUS确定
*/
static uint16 _SdSetDataBus(void)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    stCmdSendDef stCmdSend = {0};

    if (SdCdStaTran != enCurCdSta)
    {
        err = 0xff;
        ucStep = 0;
        goto ErrReTurn;
    }
    /*cmd0*/
    stCmdSend.ulCmd  = SD_CMD_SET_BUS_WIDTH | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    stCmdSend.ulArg  = SD_DATA_BUS;
    err = SdSendAppCmd(&stCmdSend);
    if (err)
    {
        ucStep = 1;
        goto ErrReTurn;
    }

    if (_SdChckR1(CS_ERROR) || (0 == _SdChckR1(SD_STATE_APP_CMD)))
    {
        err = 0xfe;
        ucStep = 2;
        goto ErrReTurn;
    }
    return 0;
ErrReTurn:
    PrintErr(_SdSetDataBus, ucStep, err);
    return err;
}

/*
功能:
    1.发送Switch Function Command(CMD6)
参数:   void
返回值: 
    0:      Success
    Others: Error Code
输入:   void
输出:   void
备注:   void
注意:   
    1.只有Physical Layer Specification Version 1.10及其兼容的SD卡支持CMD6
    2.CMD6 is valid under the "Transfer State". 
    3. this is a standard single block read transaction,
*/
static uint16 _SdSendSwichFunc(enSdSFModeDef mode)
{
    uint16   err = 0;
    stCmdSendDef stTmp = {0};
    uint8 ucStep = 0;

    {
        SDIO_DataInitTypeDef SDIO_DataInitStruct = {0};
    
        SDIO_DataInitStruct.SDIO_DataTimeOut     = SD_DTIMER_TIME * 600;//100ms timeout
        SDIO_DataInitStruct.SDIO_DataLength      = 512 / 8;
        SDIO_DataInitStruct.SDIO_DataBlockSize   = SDIO_DataBlockSize_64b;
        SDIO_DataInitStruct.SDIO_TransferDir     = SDIO_TransferDir_ToSDIO;
        SDIO_DataInitStruct.SDIO_TransferMode    = SDIO_TransferMode_Block;
        SDIO_DataInitStruct.SDIO_DPSM            = SDIO_DPSM_Enable;
    //SDIO->CLKCR &= ~SDIO_BusWide_4b ;
        SDIO_DataConfig(&SDIO_DataInitStruct);
    }
    /*cmd0*/
    stTmp.ulArg  = ((uint32)mode << 31) | 0x01;
    stTmp.ulCmd  = SD_CMD_SWITCH_FUNC | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdioSendRegularCmd(&stTmp);
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }
    
    if (_SdChckR1(CS_ERROR))
    {
        err = 0xff;
        ucStep = 2;
        goto ErrReturn;
    }
    {
        uint8 aucTmp[512] = {0};
    
        if(SdioRwData(enRead, aucTmp, sizeof(aucTmp)))
        {
            err = 0xfd;
            ucStep = 3;
            goto ErrReturn;
        }
    }
    return 0;
ErrReturn:
    PrintErr(_SdSendSwichFunc, ucStep, err);
    return err;

}

static bool _SdChckSwData(void)
{
    return 0;
}

/*检查当前卡是否支持high speed mode
false   不支持
true    支持
*/
bool _SdChckFunc(void)
{
    uint16   err = 0;

    err = _SdSendSwichFunc(SD_SfMode0);
    if (0 == err)
    {
        err = _SdChckSwData();
    }
    return (err == 0) ? TRUE : FALSE;
}
#if 0
/*Tran->Tran*/
static uint16 _SdSwitchFunc(void)
{
    uint16   err = 0;//SDIO_STA_CMD_ERR;


    if (SdCdStaTran == enCurCdSta)
    {
        if (_SdChckFunc())
        {
            _SdSendSwichFunc(SD_SfMode1);
        }
    }
ErrReturn:
    return err;
}
#endif
void _SdConvertSdSta(void)
{
    uint8 *pucTmp = &unSdSta.DATA[0];
    
    memcpy(pucTmp + 14, pucTmp, 14);//借缓存
    pucTmp += 14;//指向数据头
    
    unSdSta.SdSta.DAT_BUS_WIDTH = (pucTmp[0] >> 6) & 0x03;
    unSdSta.SdSta.SECURED_MODE  = (pucTmp[0] >> 5) & 0x01;
    pucTmp += 2;
    
    unSdSta.SdSta.SD_CARD_TYPE  = ((uint16)pucTmp[0] << 8) | pucTmp[1];
    pucTmp += 2;
    
    unSdSta.SdSta.SIZE_OF_PROTECTED_AREA  = ((uint32)pucTmp[0] << 24) | ((uint32)pucTmp[1] << 16) | ((uint32)pucTmp[2] << 8) | ((uint32)pucTmp[3] << 0);
    pucTmp += 4;

    unSdSta.SdSta.SPEED_CLASS           = pucTmp[0];
    unSdSta.SdSta.PERFORMANCE_MOVE      = pucTmp[1];
    pucTmp += 2;

    unSdSta.SdSta.AU_SIZE   = (pucTmp[0] >> 4) & 0x0f;
    pucTmp++;
    
    unSdSta.SdSta.ERASE_SIZE   = ((uint16)pucTmp[0] << 8) | pucTmp[1];
    pucTmp += 2;

    unSdSta.SdSta.ERASE_TIMEOUT = (pucTmp[0] >> 2) & 0x3f;
    unSdSta.SdSta.ERASE_OFFSET  = pucTmp[0] & 0x03;
    return;
}

uint16 _SdGetSdSta(void)
{//_SdGetFiFoData
    uint16 err = 0;
    
    err = SdioRwData(enRead, unSdSta.DATA, sizeof(unSdSta.DATA));
    if (0 == err)
    {
        _SdConvertSdSta();
    }
    return err;
}

static uint16 _SdAskSdSta(void)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    stCmdSendDef stCmdSend = {0};

    if (SdCdStaTran != enCurCdSta)
    {
        err = 0xff;
        ucStep = 0;
        goto ErrReturn;
    }
    
    {
        SDIO->DTIMER = SD_DTIMER_TIME * 500;
        SDIO->DLEN   = SD_STA_SZ;
        SDIO->DCTRL = SDIO_DPSM_Enable | SDIO_DataBlockSize_64b | SDIO_TransferDir_ToSDIO | SDIO_DmaEN | SDIO_TransferMode_Block;
    }
    /*acmd13*/
    stCmdSend.ulArg  = SD_CMD_ARG_NULL ;
    stCmdSend.ulCmd  = SD_CMD_SD_STATUS | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdSendAppCmd(&stCmdSend);
    if (err)
    {
        ucStep = 1;
        goto ErrReturn;
    }
    
    if (_SdChckR1(CS_ERROR) || (0 == _SdChckR1(SD_STATE_APP_CMD)))
    {
        err = 0xfe;
        ucStep = 2;
        goto ErrReturn;
    }
    
    err = _SdGetSdSta();
    if (err)
    {
        ucStep = 3;
        goto ErrReturn;
    }
    UartPrintBuffer(unSdSta.DATA, sizeof(unSdSta));
    return 0;
ErrReturn:
    PrintErr(_SdAskSdSta, ucStep, err);
    return err;
}

#if SD_DBG
void SdBlockRead(uint32 ulBlockNum)
{
    uint16   err = 0;
    stCmdSendDef stCmdSend = {0};
    uint8    ucStep = 0;

    if (SdCdStaTran != enCurCdSta)
    {
        err = 0xff;
        ucStep = 0;
        goto ErrReturn;
    }
    
    SdioDataRegCfg(SD_DATA_BLOCK_SZ, enRead, SdBlock);
    /*acmd17*/
    stCmdSend.ulArg   = ulBlockNum ;
    stCmdSend.ulCmd   = SD_CMD_READ_SINGLE_BLOCK | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdioSendRegularCmd(&stCmdSend);
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }
    
    if (_SdChckR1(CS_ERROR))
    {
        err = 0xfd;
        ucStep = 2;
        goto ErrReturn;
    }

    err = SdioRwData(enRead, aucSdReadBuffer, sizeof(aucSdReadBuffer));
    if(err)
    {
        ucStep = 3;
        goto ErrReturn;
    }
    //UartPrintBuffer(aucSdReadBuffer, sizeof(aucSdReadBuffer));
    printf("ReadData:%s\r\n", aucSdReadBuffer);
    return;
ErrReturn:
    PrintErr(SdBlockRead, ucStep, err);
    return;

}

void SdBlockWrite(uint32 ulBlockNum)
{
    stCmdSendDef stCmdSend = {0};
    uint16   err = 0;
    uint8    ucStep = 0;

    err = _SdAskCardSta();
    if(err || (SdCdStaTran != enCurCdSta))
    {
        ucStep = 0;
        goto ErrReturn;
    }
        /*acmd24*/
    stCmdSend.ulArg  = ulBlockNum ;
    stCmdSend.ulCmd  = SD_CMD_WRITE_BLOCK | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdioSendRegularCmd(&stCmdSend);
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }
    
    if (_SdChckR1(CS_ERROR))
    {
        err = 0xfd;
        ucStep = 2;
        goto ErrReturn;
    }
    SdioDataRegCfg(SD_DATA_BLOCK_SZ, enWrite, SdBlock);
    if (SdioRwData(enWrite, aucSdWriteBuffer, sizeof(aucSdWriteBuffer)))
    {
        err = 0xfb;
        ucStep = 3;
        goto ErrReturn;
    }
    //_SdStopTran();
    return;
ErrReturn:
    PrintErr(SdBlockWrite, ucStep, err);
    return;
}
#endif
static uint16 SdPreEarsed(uint32 ulBlockNum)
{
    stCmdSendDef stCmdSend = {0};
    uint16   err = 0;
    uint8    ucStep = 0;

    if (SdCdStaTran != enCurCdSta)
    {
        err = 0xff;
        ucStep = 0; 
        goto ErrReturn;
    }
        /*acmd23*/
    stCmdSend.ulArg  = ulBlockNum ;
    stCmdSend.ulCmd  = SD_CMD_SET_WR_BLK_ERASE_COUNT | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdSendAppCmd(&stCmdSend);
    if (err)
    {
        ucStep = 1; 
        goto ErrReturn;
    }
    
    if (_SdChckR1(CS_ERROR) || (0 == _SdChckR1(SD_STATE_APP_CMD)))
    {
        err = 0xfd;
        ucStep = 2; 
        goto ErrReturn;
    }
    return 0;
ErrReturn:
    PrintErr(SdPreEarsed, ucStep, err);
    return err;
}

static uint16 _SdStopTran(void)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    stCmdSendDef stCmdSend = {0};

    err = _SdAskCardSta();
    if(err)
    {
        ucStep = 0;
        goto ErrReTurn;
    }
    /*cmd12*/
    stCmdSend.ulCmd  = SD_CMD_STOP_TRANSMISSION | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    stCmdSend.ulArg  = SD_CMD_ARG_NULL;
    err = SdioSendRegularCmd(&stCmdSend);
    if (err)
    {
        ucStep = 1;
        goto ErrReTurn;
    }
    err = _SdChckR1b();
    if (err)
    {
        ucStep = 2;
        goto ErrReTurn;
    }
    err = _SdAskCardSta();
    if (err)
    {
        err = 0xfd;
        ucStep = 3;
        goto ErrReTurn;
    }
    return 0;
ErrReTurn:
    PrintErr(_SdStopTran, ucStep, err);
    return err;
}

/*发送ONE BLOCK 数据*/
static uint16 _SdSendOneBlockData(uint8 * const buff)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    __align(4) static uint8 saucTmp[SD_DATA_BLOCK_SZ] __attribute__((at(0x20000050))) = {0};

    memcpy(saucTmp, buff, sizeof(saucTmp));
    SdioDataRegCfg(SD_DATA_BLOCK_SZ, enWrite, SdBlock);
    err = SdioRwData(enWrite, saucTmp, SD_DATA_BLOCK_SZ);
    if (err)
    {
        ucStep = 0;
        goto ErrReturn;
    }
    return err;
ErrReturn:
    PrintErr(_SdSendOneBlockData, ucStep, err);
    return err;
}

static uint16 SdSendMultiWriteCmd(
	const uint8 * buff,		/* Data buffer to store read data */
	uint32 sector,	/* Start sector in LBA */
	uint32 count		/* Number of sectors to read */
)
{
    stCmdSendDef stCmdSend = {0};
    uint16   err = 0;
    uint8    ucStep = 0;
    
    if (SdCdStaTran != enCurCdSta)
    {
        err = 0xff;
        ucStep = 0;
        goto ErrReturn;
    }
        /*cmd25*/
    stCmdSend.ulArg  = sector ;
    stCmdSend.ulCmd  = SD_CMD_WRITE_MULTIPLE_BLOCK | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdioSendRegularCmd(&stCmdSend);
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }

    err = _SdChckR1(CS_ERROR);
    if (err)
    {
        ucStep = 2;
        goto ErrReturn;
    }
    while(count--)
    {
        _SdWaitDataW();
        _SdSendOneBlockData((uint8 *)buff);
        buff += SD_DATA_BLOCK_SZ;
    }
    return err;
ErrReturn:
    PrintErr(SdSendMultiWriteCmd, ucStep, err);
    return err;
}

static uint16 SdSendMultiReadCmd(
	uint8 *buff,		/* Data buffer to store read data */
	uint32 sector,	/* Start sector in LBA */
	uint32 count		/* Number of sectors to read */
)
{
    stCmdSendDef stCmdSend = {0};
    uint16   err = 0;
    uint8    ucStep = 0;
    uint32   ulDataLen = count * SD_DATA_BLOCK_SZ;

    /*cmd18*/
    stCmdSend.ulArg  = sector ;
    stCmdSend.ulCmd  = SD_CMD_READ_MULTIPLE_BLOCK | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdioSendRegularCmd(&stCmdSend);
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }

    err = _SdChckR1(CS_ERROR);
    if (err)
    {
        ucStep = 2;
        goto ErrReturn;
    }
#if 0
    while(count--)
    {
        _SdSendOneBlockData(enRead, buff);
        buff += SD_DATA_BLOCK_SZ;
    }
#else
    SdioDataRegCfg(ulDataLen, enRead, SdBlock);
    err = SdioRwData(enRead, buff, ulDataLen);
if (err)
{
    ucStep = 0;
    goto ErrReturn;
}
#endif
    return err;
ErrReturn:
    PrintErr(SdSendMultiReadCmd, ucStep, err);
    return err;
}

static uint16 SdSendEarseCmd(
	enSdEarseCmdDef cmd,		/* Data buffer to store read data */
	uint32 sector	/* Start sector in LBA */
)
{
    stCmdSendDef stCmdSend = {0};
    uint16   err = 0;
    uint8    ucStep = 0;

    /*cmd18*/
    stCmdSend.ulArg  = sector ;
    switch(cmd)
    {
        case SdEarseStart :
        {
            stCmdSend.ulCmd  = SD_CMD_ERASE_WR_BLK_START | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
            break;
        }
        case SdEarseStop :
        {
            stCmdSend.ulCmd  = SD_CMD_ERASE_WR_BLK_END | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
            break;
        }
        case SdEarseAct :
        {
            stCmdSend.ulCmd  = SD_CMD_SEND_ERASE | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
            break;
        }
        default :break;
    }
    err = SdioSendRegularCmd(&stCmdSend);
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }

    err = _SdChckR1(CS_ERROR);
    if (err)
    {
        ucStep = 2;
        goto ErrReturn;
    }
    return err;
ErrReturn:
    PrintErr(SdSendEarseCmd, ucStep, err);
    return err;
}

/*获取已经写入成功的block数*/
uint16 SdGetWritenBlockNum(uint32 *ulBlockNum)
{
    uint16   err = 0;
    uint32   ulTmp = 0;
    uint8    ucStep = 0;
    stCmdSendDef stCmdSend = {0};
    

    if (SdCdStaTran != enCurCdSta)
    {
        err = 0xff;
        ucStep = 1;
        goto ErrReturn;
    }
    
    {
        SDIO->ICR = SDIO_DERR_CLR;
        SDIO->DTIMER = SD_DTIMER_TIME * 500;
        SDIO->DLEN   = BIT32_SZ;//32bit
        SDIO->DCTRL  = SDIO_DPSM_Enable | SDIO_DataBlockSize_4b | SDIO_TransferDir_ToSDIO | SDIO_DmaEN | SDIO_TransferMode_Block;
    }
    /*acmd22*/
    stCmdSend.ulArg   = SD_CMD_ARG_NULL;
    stCmdSend.ulCmd   = SD_CMD_SEND_NUM_WR_BLOCKS | SDIO_CPSM_Enable | SDIO_Response_Short | SDIO_Wait_No;
    err = SdSendAppCmd(&stCmdSend);
    if (err)
    {
        ucStep = 2;
        goto ErrReturn;
    }
    
    if (_SdChckR1(CS_ERROR) || (0 == _SdChckR1(SD_STATE_APP_CMD)))
    {
        err = 0xfd;
        ucStep = 3;
        goto ErrReturn;
    }
    
    err = SdioRwData(enRead, (uint8 *)&ulTmp, BIT32_SZ);
    if(err)
    {
        ucStep = 4;
        goto ErrReturn;
    }
    Little2BigW(*ulBlockNum, ulTmp);
    return 0;
ErrReturn:
    PrintErr(SdGetWritenBlockNum, ucStep, err);
    return err;
}

uint16 SdMultiBlockWrite(
	const uint8 *buff,		/* Data buffer to store read data */
	uint32 sector,	/* Start sector in LBA */
	uint32 count		/* Number of sectors to read */
)
{
    uint16   err = 0;
    uint8    ucStep = 0;
    uint32    ulWritenNum = 0;//writen success block size


    err = _SdAskCardSta();
    if(err || (SdCdStaTran != enCurCdSta))
    {
        ucStep = 0;
        goto ErrReturn;
    }
    
    err = SdPreEarsed(count);
    if(err)
    {
        ucStep = 2;
        goto ErrReturn;
    }
    
    err = SdSendMultiWriteCmd(buff, sector, count);
    if(err)
    {
        ucStep = 3;
        goto ErrReturn;
    }

    err = _SdStopTran();
    if(err)
    {
        ucStep = 4;
        goto ErrReturn;
    }

    err = SdGetWritenBlockNum(&ulWritenNum);
    if (err)
    {
        ucStep = 5;
        goto ErrReturn;
    }

    if(ulWritenNum != count)
    {
        err = 0xfd;
        ucStep = 6;
        goto ErrReturn;
    }
    return 0;
ErrReturn:
    PrintErr(SdMultiBlockWrite, ucStep, err);
    return err;
}

uint16 SdMultiBlockRead(
	uint8 *buff,		/* Data buffer to store read data */
	uint32 sector,	/* Start sector in LBA */
	uint32 count		/* Number of sectors to read */
)
{
    uint16   err = 0;
    uint8    ucStep = 0;

    err = _SdAskCardSta();
    if(err || (SdCdStaTran != enCurCdSta))
    {
        ucStep = 0;
        goto ErrReturn;
    }
    
    err = SdSendMultiReadCmd(buff, sector, count);
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }

    err = _SdStopTran();
    if(err)
    {
        ucStep = 2;
        goto ErrReturn;
    }
    return 0;
ErrReturn:
    PrintErr(SdMultiBlockRead, ucStep, err);
    return err;
}

uint16 SdMultiBlockEarse(
	uint32 sector	/* Start sector in LBA */
)
{
    uint16   err = 0;
    uint8    ucStep = 0;

    err = _SdAskCardSta();
    if(err || (SdCdStaTran != enCurCdSta))
    {
        ucStep = 0;
        goto ErrReturn;
    }
    
    err = SdSendEarseCmd(SdEarseStart, sector);//设置earse start addr
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }
    
    err = SdSendEarseCmd(SdEarseStop, sector + SD_BLOCK_SIZE);//设置earse start addr
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }

    err = SdSendEarseCmd(SdEarseAct, 0);//设置earse start addr
    if(err)
    {
        ucStep = 1;
        goto ErrReturn;
    }
    
    err = _SdAskCardSta();
    if(err || (SdCdStaTran != enCurCdSta))
    {
        ucStep = 0;
        goto ErrReturn;
    }
    return 0;
ErrReturn:
    PrintErr(SdMultiBlockEarse, ucStep, err);
    return err;
}

#if SD_DBG
void SdTest(void)
{
    SdCardInit();
    SdBlockRead(0);
    SdBlockWrite(0);
    SdMultiBlockWrite(aucSdWriteBuffer, 0x01, 0x03);
    SdMultiBlockRead(aucSdReadBuffer, 0x01, 0x03);
    return;
}
#endif

#endif
