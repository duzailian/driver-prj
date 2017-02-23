#ifndef __S50_CMD_H
#define __S50_CMD_H

#ifdef __cplusplus
 extern "C" {
#endif 

/*ATQA PICC 对REQA和WUPA的应答*/
/*Coding of ATQA*/
#pragma pack(1)
typedef struct{
uint16 AntiColl  :5;//Bit frame anticollision
uint16 RFU0       :1;//Reserved for Future ISO/IEC Use
uint16 UidSz      :2;//UID size
uint16 PTY        :4;//Proprietary coding
uint16 RFU1       :4;//Reserved for Future ISO/IEC Use
}stS50AtqaDef;
#pragma pack()

#define CMD_REQ_ID              0x08

/**************************************数据包组成**************************************************/
/*************Short frame***************/
#define SHORT_FRAME_LEN         0x07//Short frame长度(单位Bit)
/*************Short frame***************/

#define S50_LOOP_MAX        32//The maximum number of loops will be 32

/****************ANTICOLLISION/SELECT Commands****************/
/*
    (详细内容参考"S50笔记"目录下的"UID.png和"select etc.png")
    cascade level 1:
    ANTICOLLISION: (SEL 1Byte) + (NVB 1Byte) + (UID 4Byte) + (BCC 1Byte)
    SEL(1Byte) + NVB(1Byte) + UID0-UID3(UID size = 1)或CT+UID0-UID2(UID size > 1) + BCC(1Byte)
    SELECT:(SEL 1Byte) + (NVB 1Byte) + (UID 4Byte) + (BCC 1Byte) + (CRC_A 2Byte)
    SEL(1Byte) + NVB(1Byte) + UID0-UID3(UID size = 1)或CT+UID0-UID2(UID size > 1) + BCC(1Byte) + CRC_A

    cascade level 2:(UID size > 1)
    SEL(1Byte) + NVB(1Byte) + UID3-UID6(UID size = 2)或CT+UID3-UID5(UID size = 3) + BCC(1Byte)
    
    cascade level 3:(UID size = 3)
    SEL(1Byte) + NVB(1Byte) + UID6-UID9(UID size = 3) + BCC(1Byte)
*/
#define SEL_POS                 0
#define NVB_POS                 1
#define UID_POS                 2//UID或CT起始位置
#define BCC_POS                 6

#define ANTICOLL_FRAME_LEN      0x07//Bit oriented anticollision frame长度(单位Byte)
#define S50_CLN_SZ              0x05//一次Anticollision loop cascade level完成时接收到的来自PICC的数据长度
#define S50_COLL_SEL            1//位冲突出现时选择的冲突位的值

#define S50_UID_MAX_SZ          10

/*****************************************CMD*************************************************/
/*REQA/WUPA*/
#define CMD_REQA                0x26
#define CMD_WUPA                0x52

/*SEL level1-3*/
#define CMD_SEL1                0x93
#define CMD_SEL2                0x95
#define CMD_SEL3                0x97
uint8 aucSelCnvt[] = {CMD_SEL1, CMD_SEL2, CMD_SEL3};
#define GetSEL(level)           aucSelCnvt[level - 1]//lecel 1-3
/*NVB*/
/*ByteCnt 接收到的有效bit总数/8
BitCnt    接收到的有效bit总数%8
*/
#define GetNVB(ByteCnt, BitCnt)  ((ByteCnt << 4) | (BitCnt))

#define CMD_HLTA                0x50                       

#define CMD_PER_UID             0x40//Personalize UID Usage 

#define CMD_SET_MOD_TYPE        0x43//SET_MOD_TYPE

#define ATQA_SZ                  2
#define SAK_SZ                   1//不含CRC
 
typedef enum {
AntiCollCmp,//Anticollision loop 完成
AntiCollOk,//Anticollision loop leveln 完成，
AntiColling,//Anticollision loop leveln 正在进行
AntiCollErr,//Recv Success
}enS50AcStaDef;//Anticollision loop State
    

/****************************CMD ACK***********************************/
#define SAK_UID_NO_COMP         0x04//UID not complete
#define SAK_UID_COMP_OK         0x08//UID complete,PICC compliant with ISO/IEC 14443-4

#define CLN_MAX                 3//Cascade Level 最大值
#define MFAuthentTimes          3//三次认证

/************************MIFARE Authentication**********************************/    
#define CMD_AUT_KEYA            0x60//Authentication with Key A 
#define CMD_AUT_KEYB            0x61//Authentication with Key B

#define CMD_MIFARE_READ         0x30//MIFARE Read 
#define CMD_MIFARE_WRITE        0xA0//MIFARE Write
#define CMD_MIFARE_DNC          0xC0//MIFARE Decrement 
#define CMD_MIFARE_INC          0xC1//MIFARE Increment
#define CMD_MIFARE_RESTORE      0xC2//MIFARE Restore
#define CMD_MIFARE_TRANSFER     0xB0//MIFARE Transfer 

/************************MIFARE ACK and NAK**********************************/    
typedef enum {
AckOk = 0x0a,//Ack Success
NakIoptVt = 0x00,//invalid operation Transfer Buffer valid
NakIcrcVt = 0x01,//parity or CRC error Transfer Buffer valid
NakIoptIt = 0x04,//invalid operation Transfer Buffer invalid valid
NakIcrcIt = 0x05,//parity or CRC error Transfer Buffer invalid valid
NakTimeout = 0x06,//超时未收到应答
}S50MF_ACK_Def;

#ifdef __cplusplus
}
#endif


#endif /* __S50_CMD_H */




