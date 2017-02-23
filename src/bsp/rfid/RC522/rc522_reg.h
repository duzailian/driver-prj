#ifndef __RC522_REG_H
#define __RC522_REG_H

#ifdef __cplusplus
 extern "C" {
#endif 

typedef enum{
    /*Command and status*/
    RegCmd   = 1,
    RegComlEn,
    RegDivIEn,
    RegComReq,
    RegDivReq,//5
    RegErr,
    RegStatus1,
    RegStatus2,
    RegFIFOData,
    RegFIFOLevel,
    RegWaterLevel,
    RegControl,
    RegBitFraming,
    RegColl,

    /*Command*/
    RegMode = 0x11,
    RegTxMode,
    RegRxMode,
    RegTxCtrl,
    RegTxASK,
    RegTxSel,
    RegRxSel,
    RegRxThrd,
    RegDemod,
    RegMfTx = 0x1c,
    RegMfRx,
    RegSerialSpeed = 0x1f,
    
    /*Configuration*/
    RegCrcRstH = 0x21,
    RegCrcRstL,
    RegModWidth = 0x24,
    RegRFCfg    = 0x26,
    RegGsN,
    RegCWGsP,
    RegModGsP,
    RegTMode,
    RegTPrescaler,
    RegTReloadH,
    RegTReloadL,
    RegTCounterValH,
    RegTCounterValL,
    
    /*Test register*/
    TestSel1Reg = 0x31,
    TestSel2Reg,
    TestPinEnReg,
    TestPinValueReg,
    TestBusReg,
    AutoTestReg,
    VersionReg,
    AnalogTestReg,
    TestDAC1Reg,
    TestDAC2Reg,
    TestADCReg,
}enRc522RegDef;
#define CommandReg   //register      
#define PwrDown         (0x01 << 4)
#define RcvOff          (0x01 << 5)
#define CmdMsk          0x0f

#define ComIEnReg //register
#define TimerIEn        (0x01 << 0)
#define ErrIEn          (0x01 << 1)
#define LoAlertIEn      (0x01 << 2)
#define HiAlertIEn      (0x01 << 3)
#define IdleIEn         (0x01 << 4)
#define RxIEn           (0x01 << 5)
#define TxIEn           (0x01 << 6)
#define IRqInv          (0x01 << 7)

#define DivIEnReg //register
#define CRCIEn          (0x01 << 2)
#define MfinActIEn      (0x01 << 4)
#define IRQPushPull     (0x01 << 7)

#define ComIrqReg //register
#define TimerIRq        (0x01 << 0)
#define ErrIRq          (0x01 << 1)
#define LoAlertIRq      (0x01 << 2)
#define HiAlertIRq      (0x01 << 3)
#define IdleIRq         (0x01 << 4)
#define RxIRq           (0x01 << 5)
#define TxIRq           (0x01 << 6)
#define Set1            (0x01 << 7)

#define DivIrqReg //register
#define CRCIRq          (0x01 << 2)
#define MfinActIRq      (0x01 << 4)
#define Set2            (0x01 << 7)

#define ErrorReg //register
#define ProtocolErr      (0x01 << 0)
#define ParityErr        (0x01 << 1)
#define CRCErr           (0x01 << 2)
#define CollErr          (0x01 << 3)
#define BufferOvfl       (0x01 << 4)
#define TempErr          (0x01 << 6)
#define WrErr            (0x01 << 7)

#define Status1Reg //register
#define LoAlert         (0x01 << 0)
#define HiAlert         (0x01 << 1)
#define TRunning        (0x01 << 3)
#define IRq             (0x01 << 4)
#define CRCReady        (0x01 << 5)
#define CRCOk           (0x01 << 6)

#define Status2Reg //register
#define StateMask         (0x07)//ModemState
#define MFCrypto1On       (0x01 << 3)//this bit is cleared by software
#define I2CForceHS        (0x01 << 6)
#define TempSensClear     (0x01 << 7)

#define FIFOLevelReg //register
#define FlushBuffer       (1 << 7)/* immediately clears the internal FIFO buffer’s read and write pointer 
                                              and ErrorReg register’s BufferOvfl bit
                                              reading this bit always returns 0 */
#define FIFO_SzMsk          (BitMask(7))//

#define ControlReg //register
#define TStartNow                   (1 << 6)//timer start
#define TStopNow                    (1 << 7)//timer stop
#define RxBitsMask                  (7)//indicates the number of valid bits in the last received byte
    
#define BitFramingReg //register
#define StartSend                    (1 << 7) 
#define RxAlign                      (7 << 4) 
#define TxLastBits                   (7 << 0) 
#define TX_WHOLE_BYTE                0

#define CollReg //register
#define CollClr                      (0 << 7)//遇到冲突位，清除所有接收数据
#define CollNoClr                    (1 << 7)//遇到冲突位，不清除所有接收数据
/* ↓no collision detected  or the position of the collision is 
out of the range of CollPos[4:0]*/
#define CollPosNotValid               (1 << 5)
#define CollPosMask                   (0x1f)

#define ModeReg //register
#define MSBFirst                    (1 << 7)//CRC coprocessor calculates the CRC with MSB first
/*transmitter can only be started if an RF field is generated*/
#define TxWaitRF                    (1 << 5)
#define PolMFin                     (1 << 3)
#define Crc0                        (0)
#define Crc6363                     (1)
#define CrcA671                     (2)
#define CrcFFFF                     (3)

#define TxModeReg //register
#define TxCRCEn                     (1 << 7)
#define TxSpeed106k                 (0 << 4)
#define TxSpeed212k                 (1 << 4)
#define TxSpeed424k                 (2 << 4)
#define TxSpeed848k                 (3 << 4)
#define InvMod                       (1 << 3)//modulation of transmitted data is inverted

#define RxModeReg //register
#define RxCRCEn                     (1 << 7)
#define RxSpeed106k                 (0 << 4)
#define RxSpeed212k                 (1 << 4)
#define RxSpeed424k                 (2 << 4)
#define RxSpeed848k                 (3 << 4)
/*↓an invalid received data stream (less than 4 bits received) will 
be ignored and the receiver remains active*/
#define RxNoErr                     (1 << 3)
#define RxMultiple                  (1 << 2)

#define TxControlReg //register
/*TxControlReg register*/
#define InvTx2RFOn                  (1 << 7)
#define InvTx1RFOn                  (1 << 6)
#define InvTx2RFOff                 (1 << 5)
#define InvTx1RFOff                 (1 << 4)
#define Tx2CW                       (1 << 3)
#define Tx2RFEn                     (1 << 1)
#define Tx1RFEn                     (1 << 0)

#define TxASKReg //register
/*TxASKReg register*/
#define Force100ASK                 (1 << 6)

#define TxSelReg //register
/*TxSelReg register*/
#define DriverSel                   (1 << 4)//modulation signal (envelope) from the internal encoder, 
#define MFOutSel                    (0)//selects the input for pin MFOUT 3-state


#define RxSelReg //register
/*RxSelReg register*/
#define UARTSel                     (2 << 6)//selects the input of the contactless UART
#define RxWait                      (2)

#define RxThresholdReg //register
/*RxThresholdReg register*/
#define MinLevel                    (2 << 4)//selects the input of the contactless UART
#define CollLevel                   (2 << 0)//selects the input of the contactless UART

#define DemodReg //register
/*DemodReg register*/

#define MfTxReg //register
/*MfTxReg register*/

#define MfRxReg //register
/*MfRxReg register*/
#define ParityDisable                (1 << 4)

#define RFCfgReg //register
/*RFCfgReg register*/
#define RxGain                       (7 << 4)

#define TModeReg //register
/*TModeReg registers*/
#define TAuto                        (1 << 7)//timer starts automatically at the end of the transmission in 
/* timer automatically restarts its count-down from the 16-bit 
timer reload value instead of counting down to zero */
#define TAutoRestart                 (1 << 4)
#define TPresHiMask                  (0x0f)

#define TPrescaler //register
/*TPrescaler registers*/
#define TPresLiMask                  (0xff)

#define TReloads //register
/*TReloads registers*/
#define TReloadHiMask                (0xff) 
#define TReloadLiMask                (0xff)


/*TPrescalerReg registers*/


#ifdef __cplusplus
}
#endif
#endif

