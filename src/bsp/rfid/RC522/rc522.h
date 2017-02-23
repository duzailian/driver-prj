#ifndef __RC522_H
#define __RC522_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define RC522_DBG                0

#define RC522_INT_IRQn           (EXTI0_IRQn)

typedef struct{
uint8 ucBitCnt  : 3;//不完整byte 中有效的bit计数(0 - 7)
uint8 ucByteCnt : 5;//总Byte计数(含不完整Byte)
}stDataCntDef;

extern void Rc522Init(void);
extern bool Rc522ReadFIFO(uint8 *pData, uint8 ucLen);
extern void Rc522WriteFIFO(uint8 *pData, uint8 uclen);
extern bool Rc522GetValidCnt(uint8 *ucByteCnt);
extern bool Rc522IsColl(void);
extern void Rc522TxCfg(bool bIsCrc);
extern void Rc522RxCfg(bool bIsCrc, bool bIsPrty);
extern void Rc522StartTransCv(uint8 RxPos, uint8 TranLen);
extern void Rc522RfSw(enOnOffDef RecvOnOff);
extern bool Rc522WaitRxEnd(void);
extern bool Rc522MFAuthent(uint8 ucCmd, uint8 ucBlockAddr, uint8 *pucKeyA, uint8 *pucUid);
extern bool Rc522GetRanData(uint8 *pucRead);
extern bool Rc522CalcCrc(uint8 *pucData, uint8 ucLen);
extern void Rc522StopTransCv(void);
extern uint8 Rc522GetFifoLevel(void);

/*清空FIFO读写指针及FIFO BufferOvfl 标志*/
#define Rc522ClrFIFO()                        do{Rc522WriteReg(RegFIFOLevel, FlushBuffer);}while(0)

#ifdef __cplusplus
}
#endif


#endif /* __SDIO_H */

