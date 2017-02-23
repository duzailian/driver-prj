#ifndef __SPI_H
#define __SPI_H

#ifdef __cplusplus
 extern "C" {
#endif 

typedef enum{
    Spi1,
    Spi2,
    Spi3,
    SpiMax = Spi3,
}enSpiChDef;
typedef uint16 (*pSpiReadFunc)(enSpiChDef enCh, uint8 *const pucRecv, uint16 usRecvLen, uint8 const *const pucSend, uint16 usSendLen);

/*        
        SPI_InitStruct.SPI_Direction    = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStruct.SPI_Mode         = SPI_Mode_Master;
        SPI_InitStruct.SPI_DataSize     = SPI_DataSize_8b;
        SPI_InitStruct.SPI_CPOL         = SPI_CPOL_Low;
        SPI_InitStruct.SPI_CPHA         = SPI_CPHA_1Edge;
    #if HARD_NSS_EN
        SPI_InitStruct.SPI_NSS          = SPI_NSS_Hard;
    #else
        SPI_InitStruct.SPI_NSS          = SPI_NSS_Soft;
    #endif
        SPI_InitStruct.SPI_BaudRatePrescaler    = SPI_BaudRatePrescaler_8;//9M
        SPI_InitStruct.SPI_FirstBit             = SPI_FirstBit_MSB;
*/

typedef struct{    
    /*SPI*/
    enSpiChDef Ch;// spi channel Spi1 etc.
    SPI_InitTypeDef *SPI_InitStruct;//used in StdPeriph_Lib
    pSpiReadFunc    pSpiReadFunc;//考虑到不同设备的SPI函数可能不一致，此处添加SPI读函数指针
}stSpiInitDef;

void SpiInit(stSpiInitDef *stCh);
uint16 SpiDataWrite(enSpiChDef enCh, uint8 const *const pucWrite, uint16 const usDataSz);
uint16 SpiDataRead(enSpiChDef enCh, uint8 *const pucRecv, uint16 usRecvLen, uint8 const *const pucSend, uint16 usSendLen);
uint16 W25Q64SpiRead(enSpiChDef enCh, uint8 *const pucRecv, uint16 usRecvLen, uint8 const *const pucSend, uint16 usSendLen);
uint16 RC522SpiRead(enSpiChDef enCh, uint8 *const pucRecv, uint16 usRecvLen, uint8 const *const pucSend, uint16 usSendLen);
uint16 SpiDataWriteByte(enSpiChDef enCh, uint8 const ucWrite);

#ifdef __cplusplus
}
#endif
#endif
