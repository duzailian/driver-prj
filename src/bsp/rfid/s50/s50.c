#ifndef __S50_C
#define __S50_C

#ifdef __cplusplus
 extern "C" {
#endif 

#include "s50cmd.h"
#include "s50reg.h"

#define S50_REQA_INVL       200//REQA发送间隔(ms)

typedef enum{
S50StaNull,//未寻到卡
S50StaReady,//就绪态,已成功收到PICC对REQA or WUPA cmd的应答ATQA
S50StaActive,//完成卡select
S50StaHalt,
}enS50StaDef;

/*
注意:UID的存储格式如下:
    single: UID0-UID4
    double: UID0-UID6
    triple: UID0-UID9
*/
typedef struct{
uint8 ucUidSz;//UID size
uint8 aucUID[S50_UID_MAX_SZ];//UID
uint8 aucClnBuff[CLN_MAX][S50_CLN_SZ];//存储包含UID的数据缓存

uint8 aucKeyA[S50_KEYA_SZ];
}stS50InfoDef;

static bool S50SendShortFrame(uint8 ucCmd, uint8 *pucRecv, uint8 ucRecvLen);
static bool S50ChckBcc(uint8 *pucData);

static stS50InfoDef stS50Info = {0};

/*根据Anticollision loop中最后发送的数据，保存CLn数据，供Select命令使用*/
#define S50SaveClnData(level, pucData)         do{memcpy(stS50Info.aucClnBuff[level - 1], pucData, S50_CLN_SZ);}while(0)
#define S50GetClnData(level, pucData)          do{memcpy(pucData, stS50Info.aucClnBuff[level - 1], S50_CLN_SZ);}while(0)

/*从ATQA中分离出UID Size*/
static  uint8 S50UidSzCNV[] = {4, 7, 10};//ATQA 中的UID size bit frame位段0-2对应的UID size分别为4,7,10
#define S50GetUidSz(pucRecv)          (S50UidSzCNV[((stS50AtqaDef *)&pucRecv)->UidSz])

/*根据收到标志UID已完整发送的SAK对应的Select等级，保存UID Size到stS50Info.ucUidSz*/
#define S50SaveUidSz(level)             do{stS50Info.ucUidSz = S50UidSzCNV[level - 1];}while(0)

/*
功能:
    向S50卡发送Short frame并等待应答
参数:
    ucCmd: 命令
    pucRecv: 指向接收缓存的指针
返回值:
    TRUE:成功等到应答 
    FALSE:等待应答超时/错误
输入:void
输出:void
备注:
    1.ATQA,2Bytes,有校验无CRC
    2.由于14443-3里ATQA只有
*/
static bool S50SendShortFrame(uint8 ucCmd, uint8 *pucRecv, uint8 ucRecvLen)
{
    bool bRes = FALSE;

    Rc522RxCfg(FALSE, TRUE);
    Rc522TxCfg(FALSE);
    Rc522WriteFIFO(&ucCmd, sizeof(ucCmd));
    //Rc522RfSw(enOn);
    Rc522StartTransCv(0, SHORT_FRAME_LEN);//ShortFrame只有7bits数据
    bRes = Rc522WaitRxEnd();
    Rc522StopTransCv();
    if(bRes && (FALSE == Rc522IsColl()))
    {//收到应答且无碰撞
        bRes = Rc522ReadFIFO(pucRecv, ucRecvLen);
    }
    else
    {
        bRes = FALSE;
    }
    //Rc522RfSw(enOff);
    return bRes;
}

/*
功能:
    向S50卡发送Standard frame并等待应答
参数:
    pucRecv:  指向接收缓存的指针
    pucSend:  指向发送缓存的指针
    ucSendLen: 要发送的数据长度
返回值:
    TRUE:成功等到应答 
    FALSE:等待应答超时/错误
输入:void
输出:void
备注:
    1.发送/接收数据均包含CRC和parity 
*/
static bool S50SendStandFrame(uint8 *pucRecv, uint8 ucRecvLen, uint8 *pucSend, uint8 ucSendLen)
{
    bool bRes = FALSE;
    //uint8 aucCrc[2] = {0};

    Rc522RxCfg(TRUE, TRUE);
    Rc522TxCfg(TRUE);
    Rc522CalcCrc(pucSend, ucSendLen);    
    Rc522WriteFIFO(pucSend, ucSendLen);
    Rc522StartTransCv(0, TX_WHOLE_BYTE);//整字节发送
    bRes = Rc522WaitRxEnd();
    Rc522StopTransCv();
    if((FALSE == bRes) || Rc522IsColl())
    {//有碰撞或者Rc522WaitRxEnd()返回错误
        bRes = FALSE;
        goto ErrReturn;
    }
    bRes = Rc522ReadFIFO(pucRecv, ucRecvLen);
ErrReturn:
    //Rc522RfSw(enOff);
    return bRes;
}

/*
功能:
    向S50卡发送REQA并等待应答
参数:
    void
返回值:
    TRUE:成功等到应答 FALSE:等待应答超时
输入:void
输出:UID Size
*/
static bool S50SendREQA(void)
{
    uint8  aucTmp[ATQA_SZ] = {0};
    bool   bRes  = FALSE;

    bRes = S50SendShortFrame(CMD_REQA, &aucTmp[0], sizeof(aucTmp));
    if(bRes)
    {
        stS50Info.ucUidSz = S50GetUidSz(aucTmp[0]);
        Beep(BeepDouble);
        printf("S50 Card Detected!UID Size:%d\r\n", stS50Info.ucUidSz);
    }
    return bRes;
}

/*
功能:
    向S50卡发送WUPA并等待应答
参数:
    void
返回值:
    TRUE:成功等到应答 FALSE:等待应答超时
输入:void
输出:UID Size
*/
bool S50SendWUPA(void)
{
    uint8  aucTmp[ATQA_SZ] = {0};
    bool   bRes  = FALSE;

    bRes = S50SendShortFrame(CMD_WUPA, &aucTmp[0], sizeof(aucTmp));
    if(bRes)
    {
        stS50Info.ucUidSz = S50GetUidSz(aucTmp[0]);
    }
    return bRes;
}

/*
功能:
    Anticollision loop过程中根据接收到的数据，打包需要发送的数据
参数:
    1.pucSend     指向发送缓存的指针
    2.pusSendCnt  指向要发送的数据Bit计数(包含NVB和SEL)的结构指针
返回值: 
    1.TRUE:  success
    2.FALSE: fail
输入:   void
输出:   void
备注:
    1.NVB中存储的是当前防冲突帧中的有效Bytes+Bits
    2.发送数据包构成:SEL + NVB + (n + 1)Bits(其中n为接收到的bit数)
      发送包在接收包基础上在第一个冲突位选择发送0或1(S50_COLL_SEL)
注意:
    1.pstSendCnt->ucByteCnt存储写入FIFO的总数(包括不完整Byte)
    2.pstSendCnt->ucBitCnt 存储要发送的最后一个Byte中的有效Bit数
*/
static bool S50PackAcData(uint8 *pucSend, stDataCntDef *pstSendCnt)
{
    uint8  ucSendCnt     = 0;//此次接收的bits数+上次发送的Bits数(bit为单位,含SEL和NVB)
    const uint8  ucSendByteCnt = pstSendCnt->ucByteCnt;//处理此次接收到的数据前,缓存中的有效数据长度(含不完整Byte)
    const uint8  ucSendBitCnt  = pstSendCnt->ucBitCnt;//处理此次接收到的数据前,缓存中的最后一个Byte中的Bit数
    uint8  aucRecv[S50_CLN_SZ] = {0};//存储此次收到的数据
    uint8  ucNewBitCnt = 0;//新接收的有效数据长度(bit为单位)
    bool   bIsColl = FALSE;

    if(0 == ucSendBitCnt)
    {//上次发送不存在不完整的Byte,
        ucSendCnt = ucSendByteCnt * 8;
    }
    else
    {
        ucSendCnt = (ucSendByteCnt - 1) * 8 + ucSendBitCnt;
    }
    bIsColl = Rc522GetValidCnt(&ucNewBitCnt);
    ucSendCnt += ucNewBitCnt;//上次发送Bits数+此次接收的Bits数
    if(((ANTICOLL_FRAME_LEN * 8) < ucSendCnt) || ((FALSE == bIsColl) && ((ANTICOLL_FRAME_LEN * 8) != ucSendCnt)))
    {//数据长度错误
        goto ErrReturn;
    }
    {//读取FIFO中的数据，屏蔽掉无效位
        uint8 ucDataLen  = 0;
        uint8 ucBitCnt   = ucSendBitCnt + ucNewBitCnt;//接收到的第一个Bit在缓存中的位置+接收到的有效数据长度(bit为单位)
        
        if(0 == (ucBitCnt % 8))
        {
            ucDataLen = ucBitCnt / 8;
            Rc522ReadFIFO(&aucRecv[0], ucDataLen);
        }
        else
        {//接收到的数据尾存在不完整Byte ucDataLen + 1
            ucDataLen = ucBitCnt / 8 + 1;
            Rc522ReadFIFO(&aucRecv[0], ucDataLen);
            aucRecv[ucDataLen - 1] &= BitMask(ucBitCnt % 8);//屏蔽掉数据尾的无效位
        }
    }
    {//将接收到的数据粘贴到pucSend中 //将要发送的最后一个Bit设置成S50_COLL_SEL
        uint8 ucTmp = ucNewBitCnt + ucSendBitCnt;
        uint8 ucByteCnt = ucTmp / 8;//新加入的Byte个数
        uint8 ucBitCnt  = ucTmp % 8;

        if(ucBitCnt)
        {
            ucByteCnt++;
        }
        /*将接收到的数据粘贴到pucSend中*/
        if (0 == ucSendBitCnt)
        {//添加的数据前端不存在不完整Byte
            memcpy(&pucSend[ucSendByteCnt], &aucRecv[0], ucByteCnt);
        }
        else
        {
            pucSend[ucSendByteCnt - 1] |= aucRecv[0] & BitMask(ucSendBitCnt);//屏蔽掉数据头的无效位
            memcpy(&pucSend[ucSendByteCnt], &aucRecv[1], ucByteCnt - 1);
        }

        /*将要发送的最后一个Bit设置成S50_COLL_SEL*/
        if((ANTICOLL_FRAME_LEN * 8) != ucSendCnt)
        {//UCL未完整接收
            ucTmp++;
            ucByteCnt = ucTmp / 8;
            ucBitCnt  = ucTmp % 8;
            if(0 == ucBitCnt)
            {
                pucSend[ucSendByteCnt + ucByteCnt - 1] |= (S50_COLL_SEL << 7);
            }
            else
            {
                pucSend[ucSendByteCnt + ucByteCnt] |= (S50_COLL_SEL << (ucBitCnt - 1));
            }
        }
        pstSendCnt->ucBitCnt  = ucBitCnt;
        pstSendCnt->ucByteCnt = ucByteCnt + ucSendByteCnt;
        if(ucBitCnt)
        {
            pstSendCnt->ucByteCnt++;
        }
    }
    return TRUE;
ErrReturn:
    PrintErr(S50PackAcData, 00, 0xff);
    return FALSE;
}

/*
功能:
    检查S50卡Anticollision loop执行状态
参数:   void
返回值:
    AntiColling:  当前level的 Anticollision loop未完成
    AntiCollOk:   当前level的 Anticollision loop已完成(BCC校验通过)
    AntiCollErr:  Fail
输入:void
输出:void
备注:
    1.NVB中存储的是当前防冲突帧中的有效Bytes+Bits(包括NVB和SEL)
    2.防冲突帧无CRC，有parity
*/
static enS50AcStaDef S50AntiCollStaChck(uint8 *pucUid, stDataCntDef stSendCnt)
{
    enS50AcStaDef enSta = AntiCollErr;
    uint8 ucSendBitCnt = stSendCnt.ucBitCnt;//要发送的数据中最后一个字节的Bit数
    uint8 ucSendByteCnt = stSendCnt.ucByteCnt;//要发送的数据中最后一个字节的Bit数

    if (ucSendByteCnt < ANTICOLL_FRAME_LEN)
    {
        enSta = AntiColling;
        goto Return;
    }   
    else if(ucSendByteCnt > ANTICOLL_FRAME_LEN)
    {
        enSta = AntiCollErr;
        goto Return;
    }
    else if((0 == ucSendBitCnt) && S50ChckBcc(pucUid))
    {
        enSta = AntiCollOk;
        goto Return;
    }
    
Return :
    return enSta;
}

/*
功能:
    1.向S50卡发送Anticollision loop数据并等待接收完成
参数:
    1.pucSend:      指向发送缓存的指针
    2.pstSendCnt:   指向存储发送数据长度的结构体的指针
返回值:
    TRUE:  Success 
    FALSE: Fail
输入:void
输出:void
备注:
    1.存放接收到的第一个Bit的位置，恰好也是stSendCnt.ucBitCnt
*/
static bool S50SendAcData(uint8 *pucSend, stDataCntDef stSendCnt)
{
    bool bRes = FALSE;
    
    Rc522WriteFIFO(&pucSend[0], stSendCnt.ucByteCnt);
    Rc522StartTransCv(stSendCnt.ucBitCnt, stSendCnt.ucBitCnt);
    bRes = Rc522WaitRxEnd();
    Rc522StopTransCv();
    return bRes;
}

/*
功能:
    向S50卡发起防碰撞循环leveln
参数:
    ucLevel: Anticollision loop, cascade level(1-3)
返回值:
    TRUE:  Success 
    FALSE: Fail
输入:void
输出:void
备注:
    1.NVB中存储的是当前防冲突帧中的有效Bytes+Bits(包括NVB和SEL)
    2.防冲突帧无CRC，有parity
*/
static bool S50AntiCollLeveln(uint8 ucLevel)
{
    bool   res = FALSE;//返回函数执行结果
    //bool   bIfSend = FALSE;
    uint8  ucStep = 0;//返回错误步骤
    uint8  aucSend[ANTICOLL_FRAME_LEN] = {0};
    uint8  ucLoopCnt  = 1;//循环次数计数
    stDataCntDef stSendCnt = {0};//即将发送数据长度统计
    enS50AcStaDef enSta = AntiCollErr;

    Rc522RxCfg(FALSE, TRUE);
    Rc522TxCfg(FALSE);
    aucSend[SEL_POS] = GetSEL(ucLevel);
    aucSend[NVB_POS] = GetNVB(2, 0);//loop开始时ByteCnt为2，BitCnt为0
    stSendCnt.ucByteCnt = 2;//需要发送2Byte数据
    stSendCnt.ucBitCnt  = 0;//最后字节完整发送
    //Rc522RfSw(enOn);
LoopStart:
    res=  S50SendAcData(&aucSend[0], stSendCnt);
    if((FALSE == res) || (FALSE == S50PackAcData(&aucSend[0], &stSendCnt)))
    {//接收错误或数据无效
        res = FALSE;
        ucStep = 0;
        goto ErrReTurn;
    }
	enSta   = S50AntiCollStaChck(&aucSend[UID_POS], stSendCnt);
	if(AntiCollOk == enSta)
	{//本轮防碰撞流程完成
		S50SaveClnData(ucLevel, &aucSend[UID_POS]);//保存CLn数据，供Select命令使用
		goto LoopEnd;
	}
	if ((AntiCollErr == enSta) || (ucLoopCnt > S50_LOOP_MAX))
	{
		ucStep = 1;
		res = FALSE;
		goto ErrReTurn;
	}
	else
	{
		ucLoopCnt++;
		goto LoopStart;
	}
ErrReTurn:
    PrintErr(S50AntiCollLeveln, ucStep, 0);
    return FALSE;
LoopEnd:
    //Rc522RfSw(enOff);
    return TRUE;
}

/*
功能:
    calculated as exclusive-or over the 4 previous bytes, 
    判断是否BCC校验通过
参数:
    pucData: 指向要计算的起始字节的指针
返回值: 
    TRUE:  BCC检查通过
    FALSE: BCC检查未通过
输入:   void
输出:   void
备注:
*/
static bool S50ChckBcc(uint8 *pucData)
{
    uint8 ucRes = 0;
    uint8 i = 0;
    uint8 ucBcc = pucData[4];

    for(i = 0; i < 4; i++)
    {
        ucRes ^= pucData[i];
    }
    return (ucRes == ucBcc);
}

/*
功能:
    根据存储在stS50Info.aucClnBuff中的数据，整理出UID size为1的PICC的UID
参数:   void
返回值: void
输入:   void
输出:   void
备注:
    1. UID size double: [CT][UID0][UID1][UID2][BCC],[CT][UID3][UID4][UID5][BCC],[UID6][UID7][UID8][UID9][BCC]
*/
static void S50GetTripleUid(void)
{
    uint8 (*paucData)[S50_CLN_SZ] = &stS50Info.aucClnBuff[0];

    /*Copy UID*/
    memcpy(&(stS50Info.aucUID[0]), &paucData[0][1], 3);//(UID0-UID2)
    memcpy(&(stS50Info.aucUID[3]), &paucData[1][1], 3);//(UID3-UID5)
    memcpy(&(stS50Info.aucUID[6]), &paucData[2][0], 4);//(UID6-UID9)
    return;
}

/*
功能:
    根据存储在stS50Info.aucClnBuff中的数据，整理出UID size为1的PICC的UID
参数:   void
返回值: void
输入:   void
输出:   void
备注:
    1. UID size double: [CT][UID0][UID1][UID2][BCC],[UID3][UID4][UID5][UID6][BCC],
*/
static void S50GetDoubleUid(void)
{
    uint8 (*paucData)[S50_CLN_SZ] = &stS50Info.aucClnBuff[0];

    /*Copy UID*/
    memcpy(&(stS50Info.aucUID[0]), &paucData[0][1], 3);//(UID0-UID2)
    memcpy(&(stS50Info.aucUID[3]), &paucData[1][0], 4);//(UID3-UID6)
    return;
}

/*
功能:
    根据存储在stS50Info.aucClnBuff中的数据，整理出UID size为1的PICC的UID
参数:   void
返回值: void
输入:   void
输出:   void
备注:
    1.UID size single: [UID0][UID1][UID2][UID3][BCC]
*/
static void S50GetSingleUid(void)
{
    uint8 *pucData = &(stS50Info.aucClnBuff[0][0]);
    
    memcpy(&(stS50Info.aucUID[0]), pucData, 4);
    return;
}

/*
功能:
    根据存储在stS50Info.aucClnBuff中的数据，整理出UID
参数:
    ucLevel: Anticollision loop, cascade level 
             如果检查SAK后确定UID已完整获取,ucLevel即UID size
返回值: void
输入:   void
输出:   void
备注:
    1.UID size single: [UID0][UID1][UID2][UID3][BCC]
      UID size double: [CT][UID0][UID1][UID2][BCC],[UID3][UID4][UID5][UID6][BCC],
      UID size double: [CT][UID0][UID1][UID2][BCC],[CT][UID3][UID4][UID5][BCC],[UID6][UID7][UID8][UID9][BCC]
*/
static void S50GetUid(uint8 ucLevel)
{
    switch(ucLevel)
    {
        case 1:
        {
            S50GetSingleUid();
            break;
        }
        case 2:
        {
            S50GetDoubleUid();
            break;
        }
        case 3:
        {
            S50GetTripleUid();
            break;
        }
        default:
        {
            break;
        }
    }
    return;
}

/*
功能:
    检查来自PICC的SAK,判断UID是否完整接收
参数:
    void
返回值:
    UidNoCMPE: PCD未完整接收到PICC的UID
    ProtOK: PCD已完整接收到PICC的UID,且PICC符合14443-4协议
    ProtErr:PCD已完整接收到PICC的UID,但PICC不符合14443-4协议
输入:void
输出:void
*/
static enS50AcStaDef S50ChckSak(uint8 ucSAK)
{
    enS50AcStaDef stSta = AntiCollErr;

    if((ucSAK & SAK_UID_NO_COMP) == SAK_UID_NO_COMP)
    {
        stSta = AntiCollOk;
    }
    else if((ucSAK & SAK_UID_COMP_OK) == SAK_UID_COMP_OK)
    {
        stSta = AntiCollCmp;
    }
    else
    {
        stSta = AntiCollErr;
        printf("PICC is Not S50!\r\n");
    }
    return stSta;
}

/*
功能:
    打包SELECT Command数据
参数:   
    ucLevel: 当前的Anticollision loop, cascade level(1-3)
    pucData: 指向缓存的指针
返回值: 
    打包后的数据长度
输入:   void
输出:   void
备注:
*/
static uint8 S50PackSelectData(uint8 ucLevel, uint8 *pucData)
{
    uint8 i = 0;
    
    pucData[i++] = GetSEL(ucLevel);
    pucData[i++] = GetNVB(ANTICOLL_FRAME_LEN, 0);//
    S50GetClnData(ucLevel, &pucData[i]);
    return ANTICOLL_FRAME_LEN;
}

/*
功能:
    向S50卡发送SELECT Command并检查接收到的应答，如果UID完全接收完成，则返回TRUE
    否则，返回FALSE
参数:   
    ucLevel: 当前的Anticollision loop, cascade level(1-3)
返回值: 
    1.AntiCollErr:  数据接收错误
    2.AntiCollOk:   数据接收错误，但UID 传输未完成
    3.AntiCollCmp:  UID 传输完成
输入:   void
输出:   void
备注:
*/
static enS50AcStaDef S50SelectCard(uint8 ucLevel)
{
    uint8 aucSend[ANTICOLL_FRAME_LEN] = {0};
    uint8 aucRecv[SAK_SZ] = {0};
    bool  bRes  = FALSE;
    enS50AcStaDef enSta = AntiCollErr;
    
    S50PackSelectData(ucLevel, &aucSend[0]);
    bRes = S50SendStandFrame(&aucRecv[0], sizeof(aucRecv), &aucSend[0], sizeof(aucSend));
    if(FALSE == bRes)
    {
        enSta = AntiCollErr;
        goto ErrReturn;
    }
    enSta = S50ChckSak(aucRecv[0]);//接收成功
    if(AntiCollCmp == enSta)
    {
        S50GetUid(ucLevel);
    }    
ErrReturn:
    return enSta;
}


/*
功能:
    向S50卡发起防碰撞循环
参数:
    void
返回值:
    TRUE:Success FALSE:Fail
输入:void
输出:void
*/
static bool S50AntiCollLoop(void)
{
    bool res = FALSE;
    //uint8 ucStep = 0;
    uint8 i = 0;
    enS50AcStaDef enSta = AntiCollErr;

    for(i = 0; i < CLN_MAX;)
    {
        res = S50AntiCollLeveln(i + 1);
        if(FALSE == res)
        {
            goto ErrReturn;
        }
        enSta = S50SelectCard(i + 1);
        switch(enSta)
        {
            case AntiCollErr:
            {//Recv Error
                res = FALSE;
                goto ErrReturn;
            }
            case AntiCollOk:
            {//Recv ok but UID TransFer not complete 
                i++;
                res = TRUE;
                break;
            }
            case AntiCollCmp:
            {//Anticollision loop complete
                S50SaveUidSz(i + 1);
                res = TRUE;
                goto SucReturn;
            }
            default :
            {
                break;
            }
        }
    }
ErrReturn:
    return res;
SucReturn:
    return TRUE;
}

/*
功能:
    向S50卡发送HLTA命令，使其进入HLTA state
参数:
    void
返回值:
    TRUE:成功等到应答 FALSE:等待应答超时
*/
void S50SendHLTA(void)
{
    //uint8 aucTmp[] = {};
    return;
}

/*
功能:
     S50相关数据结构初始化
参数:   void
返回值: void
输入:   void
输出:   void
备注:   无
注意:   无
 */
void S50DataInit(void)
{
    uint8 ucTmp = 0;
    
    memset(&stS50Info, 0, sizeof(stS50Info));

    ucTmp = GetE2promAddr(S50KeyA);
    E2PROM_Read(ucTmp, &(stS50Info.aucKeyA[0]), S50_KEYA_SZ);//E2PROM驱动暂未完成，
    return;
}

/*
功能:
    1.向S50卡发送REQA
    2.若检测到S50卡，则执行防碰撞流程
    3.防碰撞循环完成后Select Card
参数:   void
返回值: void
输入:   void
输出:   void
备注:   无
*/
bool S50CardDetect(void)
{
    bool bRes = FALSE;
    
    while(FALSE == S50SendREQA())
    {
        DelayMs(S50_REQA_INVL);
    }
    bRes = S50AntiCollLoop();
    return bRes;
}

/*
功能:
    1.根据UID Size MF获取认证流程中的参数
参数:   
    1.pucUid 指向第ucSeq次认证时所需参数的指针
    2.ucSeq为当前认证次数(0-2)
返回值: void
输入:   void
输出:   void
备注:   无
注意:   无
*/
void S50MFGetUid(uint8 *pucUid, uint8 ucSeq)
{
    uint8 ucUidSz = stS50Info.ucUidSz;

    if (4 == ucUidSz)
    {//如果Uid Size为4，则三次认证均重复发送UID
        memcpy(pucUid, stS50Info.aucUID, 4);
    }
    else if(7 == ucUidSz)
    {
        switch(ucSeq)
        {
            case 0:
            {//第一次认证发送CL2(UID3-UID6)
                memcpy(pucUid, &(stS50Info.aucClnBuff[1][0]), 4);
                break;
            }
            case 1:
            {//第二次认证发送CL1(CT+UID0---UID2)
                memcpy(pucUid, &(stS50Info.aucClnBuff[0][0]), 4);
                break;
            }
            case 2:
            {//第三次认证发送UID0-UID3
                memcpy(pucUid, &(stS50Info.aucUID[0]), 4);
                break;
            }
            default:
            {
                break;
            }
        }
    }
    return;
}
/*
功能:
    1.向S50卡发起认证流程
    2.返回认证结果
参数:   
    1.ucBlockAddr:  需要认证的Block Address(0-63)
返回值: 
    1.TRUE:  认证通过
    2.FALSE: 认证失败
输入:   void
输出:   void
备注:   无
注意:   无
*/
bool S50MFAuthent(uint8 ucBlockAddr)
{
    uint8 *pucKeyA = stS50Info.aucKeyA;
    uint8 aucUid[4]   = {0};
    uint8 i = 0;//记录认证次数
    bool  bRes = FALSE;

    //for(i = 0; i < MFAuthentTimes; i++)
    {
        S50MFGetUid(&aucUid[0], i);
        bRes = Rc522MFAuthent(CMD_AUT_KEYA, ucBlockAddr, pucKeyA, aucUid);
        if(FALSE == bRes)
        {
            goto ErrReturn;
        }
    }
ErrReturn:
    return bRes;
}


/*
功能:
    1.等待MIFARE Classic ACK或NAK并返回等待结果
参数: void
返回值: 
    1.见S50MF_ACK_Def
输入: void
输出: void
备注: 
注意:   
*/
S50MF_ACK_Def S50MFWaitAck(void)
{
    S50MF_ACK_Def enMfAck = NakTimeout;

#if S50DBG
    if (AckOk != enMfAck)
    {
        printf("S50MFWaitAck: %d\r\n", enMfAck);
    }
#endif
    return enMfAck;
}

/*
功能:
    1.向S50卡发送MIFARE Read命令,等待应答，并将接收到的数据写入缓存
参数:   
    1.ucBlockAddr,Block地址
    2.pucRecv 指向接收到的数据的指针
返回值: 
    1.TRUE:  Success
    2.FALSE: Fail
输入:   void
输出:   void
备注:   无
注意:   
    1.S50卡每个Block存储16Bytes数据,实际有效的数据为4Byte(数据) + 1Byte(可用于存储备份Block地址)
      其余均为防错用的重复值
    2.S50卡采用小端存储
    3.出厂数据不确定
*/
static bool S50MFRecvBlockData(uint8 ucBlockAddr, uint8 *pucRecv)
{
    bool bRes = FALSE;
    uint8 aucSend[2] = {0};
    uint8 i = 0;

    aucSend[i++] = CMD_MIFARE_READ;
    aucSend[i++] = ucBlockAddr;
    bRes = S50SendStandFrame(pucRecv, S50BLOCK_DATA_SZ, aucSend, sizeof(aucSend));
    return bRes;
}

/*
功能:
    1.根据从S50卡中读取的1Block的数据，分离出4Byte有效数据
    2.返回校验结果
参数:   pucBlock,pulVal
返回值: 
    1.TRUE:  Success
    2.FALSE: Fail
输入:   
    1.pstBlockData 指向接收到的S50卡1Block数据的指针
输出:   
    1.pulVal   指向存储分离出的有效数据的指针
备注:   无
注意:   
    1.S50卡每个Block存储16Bytes数据,实际有效的数据为4Byte(数据) + 1Byte(可用于存储备份Block地址)
      其余均为防错用的重复值
    2.S50卡采用小端存储
    3.出厂数据不确定
*/
static bool S50MFUnPackReadData(stS50DataBlockDef *pstBlockData, uint32 *pulVal)
{
    {
        uint32 ulVal0 = pstBlockData->unVal0.ulWord;
        uint32 ulVal1 = pstBlockData->unVal1.ulWord;
        uint32 _ulVal = pstBlockData->_unVal.ulWord;
        //校验
        if((ulVal0 != ulVal1) || (ulVal1 != GetInvtWord(_ulVal)))
        {
            goto ErrReturn;
        }
    }
    *pulVal = pstBlockData->unVal0.ulWord;
    return TRUE;
ErrReturn:
    printf("S50 Block Data Chck Error!\r\n");
    return FALSE;
}

/*
功能:
    1.读取指定block中的内容,并返回读取结果
参数:   void
返回值: 
    1.TRUE: Success
    2.FALSE: Fail
输入:   
    1.ucBlockAddr Block Address
输出:   
    1.pulVal指向接收缓存的指针
备注:   无
注意:   
    1.S50卡每个Block存储16Bytes数据,实际有效的数据为4Byte(数据) + 1Byte(可用于存储备份Block地址)
      其余均为防错用的重复值
    2.S50卡采用小端存储
    3.出厂数据不确定
*/
bool S50MFBlockRead(uint8 ucBlockAddr, uint32 *pulVal)
{
    bool bRes = FALSE;
    uint8 aucRecv[S50BLOCK_DATA_SZ] = {0};

    bRes = S50MFRecvBlockData(ucBlockAddr, &aucRecv[0]);//发送读卡指令,并接收Block数据
    if(FALSE == bRes)
    {
        printf("S50 Block Data Recv Error!\r\n");
        goto ErrorReturn;
    }
    bRes = S50MFUnPackReadData((stS50DataBlockDef *)aucRecv, pulVal);
    return bRes;
ErrorReturn:
    return FALSE;
}

/*
功能:
    1.向S50卡发送MIFARE Write命令,等待应答，并将接收到的数据写入缓存
参数:   
    1.ucBlockAddr,Block地址
    2.pucSend 指向发送的数据的指针
返回值: 
    1.TRUE:  Success
    2.FALSE: Fail
输入:   void
输出:   void
备注:   无
注意:   
    1.S50卡每个Block存储16Bytes数据,实际有效的数据为4Byte(数据) + 1Byte(可用于存储备份Block地址)
      其余均为防错用的重复值
    2.S50卡采用小端存储
    3.出厂数据不确定
*/
static bool S50MFSendBlockData(uint8 ucBlockAddr, uint8 *pucSend)
{
    bool bRes = FALSE;
    uint8 aucSend[2] = {0};
    uint8 i = 0;

    aucSend[i++] = CMD_MIFARE_WRITE;
    aucSend[i++] = ucBlockAddr;
    bRes = S50SendStandFrame(aucSend, S50BLOCK_DATA_SZ, aucSend, sizeof(aucSend));
    return bRes;
}

/*
功能:
    1.根据ulVal获取写入S50卡1个Block的数据包
参数:   
    1.ulVal
    2.pucWrite
返回值: void
输入:   
    1.ulVal待写入block数据
输出:   
    1.pucWrite组包完成后数据指针
备注:   无MIFARE 
注意:
    1.S50卡每个Block存储4Bytes有效数据
    2.S50卡采用小端存储
    3.出厂数据不确定
*/
static void S50MFPackWriteData(uint32 ulVal, uint8 *pucSend)
{
    stS50DataBlockDef *pstBlockData = (stS50DataBlockDef *)pucSend;

    pstBlockData->unVal0.ulWord = ulVal;
    pstBlockData->_unVal.ulWord = GetInvtWord(ulVal);
    pstBlockData->unVal1.ulWord = ulVal;

    /*备份Block 暂未启用*/
    pstBlockData->ucAddr0 = pstBlockData->ucAddr1 = 0;
    pstBlockData->_ucAddr0 = pstBlockData->_ucAddr1 = GetInvtByte(0);
    return;
}
/*
功能:
    1.指定block中的内容,并返回读取结果
参数:   void
返回值: 
    1.TRUE: Success
    2.FALSE: Fail
输入:   
    1.ucBlockAddr Block Address
    2.指向接收缓存的指针
输出:   void
备注:   无MIFARE 
注意:   
    1.S50卡每个Block存储16Bytes数据,实际有效的数据为4Byte(数据) + 1Byte(可用于存储备份Block地址)
      其余均为防错用的重复值
    2.S50卡采用小端存储
    3.出厂数据不确定
*/
bool S50MFBlockWrite(uint8 ucBlockAddr, uint32 ulVal)
{
    bool bRes = FALSE;
    uint8 aucSend[S50BLOCK_DATA_SZ] = {0};

    S50MFPackWriteData(ulVal, &aucSend[0]);//整理出写入S50卡1个Block的数据
    bRes = S50MFSendBlockData(ucBlockAddr, &aucSend[0]);
    return bRes;
}

#if S50DBG
#define S50TEST_BLOCK           1
/*
功能:
    1.将stS50Info.aucKeyA填充为默认值
参数:   void
返回值: void
输入:   void
输出:   void
备注:   无
*/
void S50TestGetKeyA(void)
{
    memset(stS50Info.aucKeyA, 0xff, S50_KEYA_SZ);
    return;
}

/*
功能:
    1.S50卡读写测试
参数:   void
返回值: void
输入:   void
输出:   void
备注:   无
*/
void S50RwTest(void)
{
    uint32 ulRead = 0;
    uint32 ulWrite = 56;

    S50MFBlockRead(S50TEST_BLOCK, &ulRead);
    //S50MFBlockWrite(S50TEST_BLOCK, ulWrite);
    
    S50MFBlockRead(S50TEST_BLOCK, &ulRead);
    if(ulRead == ulWrite)
    {
        printf("S50 Read/Write Test Success!\r\n");
    }
    return;
}


/*
功能:
    1.S50卡驱动程序调试
参数:   void
返回值: void
输入:   void
输出:   void
备注:   无
*/
void S50Test(void)
{
    bool bRes = FALSE;

    S50TestGetKeyA();
Start:
    DelayMs(10);
    bRes = S50CardDetect();
    if(FALSE == bRes)
    {
        goto Start;
    }
    bRes = S50MFAuthent(S50TEST_BLOCK);
    if(FALSE == bRes)
    {
        goto Start;
    }
    S50RwTest();
    goto Start;
    return;
}
#endif

#ifdef __cplusplus
}
#endif


#endif /* __S50_C */


