#ifndef __W25Q64_C
#define __W25Q64_C

#include "w25q64cmd.h"
#include "w25q64sta.h"

/*SPI Baud rate*/
#define FLASH_BR            (SPI_BaudRatePrescaler_8)//spi 72M / 8

#define FLASH_PAGE_SZ       (256)

#define FLASH_UNIQE_ID_ZE      (64 / 8)

#define FLASH_TOTAL_SZ      (8 * 1024 * 1024)

#define FLASH_SPI_CH        (Spi2)

#define W25Q64WriteEn()              do{SpiDataWriteByte(FLASH_SPI_CH, FlashWriteEnable);}while(0)
#define W25Q64VSR_WriteEn()          do{SpiDataWriteByte(FLASH_SPI_CH, FlashVSRWriteEnable);}while(0)
#define W25Q64WriteDis()             do{SpiDataWriteByte(FLASH_SPI_CH, FlashWriteDisEnable);}while(0)

#define W25Q64EraseChip()            do{SpiDataWriteByte(FLASH_SPI_CH, FlashChipErase);}while(0)
#define W25Q64EraseSuspend()         do{SpiDataWriteByte(FLASH_SPI_CH, FlashPrgSusPend);}while(0)
#define W25Q64EraseResume()          do{SpiDataWriteByte(FLASH_SPI_CH, FlashPrgResume);}while(0)


#define W25Q64PwrDown()              do{SpiDataWriteByte(FLASH_SPI_CH, FlashPwrDown);}while(0)

#define W25Q64EnQPI()                do{SpiDataWriteByte(FLASH_SPI_CH, EnableQPI);}while(0)

#define W25Q64EnRst()                do{SpiDataWriteByte(FLASH_SPI_CH, EnableRst);}while(0)
#define W25Q64Rest()                 do{SpiDataWriteByte(FLASH_SPI_CH, FlashReset);}while(0)

typedef enum{
    enW25Q64SR1,
    enW25Q64SR2,
}enW25Q64SR;

static uint8 aucW25Q64SR[2] = {0};
static uint8 aucW25Q64UniqID[FLASH_UNIQE_ID_ZE] = {0};

enErrDef W25Q64Init(void)
{
    /*SPI init*/
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
        SPI_InitStruct.SPI_BaudRatePrescaler  = FLASH_BR;
        SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;
        SPI_InitStruct.SPI_CRCPolynomial  = 1;

        stSpiInitInfo.Ch        = FLASH_SPI_CH;
        stSpiInitInfo.SPI_InitStruct = &SPI_InitStruct;
        stSpiInitInfo.pSpiReadFunc   = W25Q64SpiRead;
        SpiInit(&stSpiInitInfo);
    }
    DelayMs(10);
    return enOk;
}

enErrDef W25Q64GetJEDECID(void)
{
    uint8 ucTmp = JEDEC_ID;
    uint8 aucTmp[3] = {0};

    if (SpiDataRead(FLASH_SPI_CH, aucTmp, sizeof(aucTmp), &ucTmp, sizeof(ucTmp)))
    {
        return enFlashRErr;
    }
    UartPrintBuffer(aucTmp, sizeof(aucTmp));
    return enOk;
}

void W25Q64ReadSR(enW25Q64SR enIndex)
{
    uint8 ucTmp = 0;
    
    if (enW25Q64SR1 ==enIndex)
    {
        ucTmp = FlashReadSR1;
    }
    else
    {
        ucTmp = FlashReadSR2;
    }
    SpiDataRead(FLASH_SPI_CH, &aucW25Q64SR[enIndex], sizeof(aucW25Q64SR[enIndex]), &ucTmp, sizeof(ucTmp));
    return;
}

void W25Q64WaitBusy(void)
{
    do
    {
        W25Q64ReadSR(enW25Q64SR1);
    }
    while(aucW25Q64SR[enW25Q64SR1] & SR1BUSY);
    return;
}

void W25Q64WriteSR(enW25Q64SR enIndex, uint16 usSRData)
{
    uint8 ucTmp[3] = {0};
    uint8 i = 0;
    
    W25Q64WriteEn();
    ucTmp[i++] = FlashWriteSR;
    ucTmp[i++] = usSRData & 0xff;
    ucTmp[i++] = (usSRData >> 8) & 0xff;
    SpiDataWrite(FLASH_SPI_CH, ucTmp, sizeof(ucTmp));
    W25Q64WaitBusy();
    return;
}

enErrDef W25Q64PagePrg(enW25Q64PrgType PrgType, uint32 ulAddr, uint8 const *const pucData, uint16 const usDataLen)
{
    enErrDef err = enOk;
    uint8 ucStep = 0;
    uint8 aucTmp[FLASH_PAGE_SZ + 4] = {0};
    const uint8 aCmd[] = {FlashPagePrg, PrgSecurityR};
    uint8 i =0;

    if ((usDataLen > FLASH_PAGE_SZ))
    {
        ucStep = 0;
        err    = enFlashLenErr;
        goto ErrReturn;
    }
    if (((usDataLen == FLASH_PAGE_SZ) && (0 != (ulAddr & 0xff))) || (ulAddr > FLASH_TOTAL_SZ))
    {
        ucStep = 0;
        err    = enFlashAddrErr;
        goto ErrReturn;
    }
    aucTmp[i++] = aCmd[PrgType];
    aucTmp[i++] = (ulAddr >> 16) & 0xff;
    aucTmp[i++] = (ulAddr >> 8)  & 0xff;
    aucTmp[i++] = (ulAddr >> 0)  & 0xff;
    memcpy(&aucTmp[i], pucData, usDataLen);
    W25Q64WriteEn();
    SpiDataWrite(FLASH_SPI_CH, aucTmp, usDataLen + 4);
    W25Q64WaitBusy();
    return enOk;
ErrReturn:
    PrintErr(W25Q64PagePrg, ucStep, err);
    return err;
}

enErrDef W25Q64PartEarse(enW25Q64EarseType  EarseType, uint32 ulAddr)
{
    enErrDef err = enOk;
    uint8 aucTmp[4] = {0};
    uint8 i = 0;
    uint8 const ucCmd[] = {Flash4kErase, Flash32kErase, Flash64kErase, EraseSecurityR};

    if (ulAddr > FLASH_TOTAL_SZ)
    {
        err    = enFlashAddrErr;
        goto ErrReturn;
    }

    aucTmp[i++] = ucCmd[EarseType];
    aucTmp[i++] = (ulAddr >> 16) & 0xff;
    aucTmp[i++] = (ulAddr >> 8)  & 0xff;
    aucTmp[i++] = (ulAddr >> 0)  & 0xff;
    W25Q64WriteEn();
    SpiDataWrite(FLASH_SPI_CH, aucTmp, sizeof(aucTmp));
    W25Q64WaitBusy();
ErrReturn:
    if(err)
    {
        PrintErr(W25Q64PartEarse, 0, err);
    }
    return err;

}

enErrDef W25Q64ReadData(enW25Q64ReadType ReadType, uint32 ulAddr, uint8 * const pucData, uint16 usDataLen)
{
    enErrDef err = enOk;
    uint8 aucTmp[5] = {0};
    uint8 i = 0;
    const uint8 aucCmd[] = {FlashReadData, FlashFastRead, ReadSecurityR};

    if (ulAddr > FLASH_TOTAL_SZ)
    {
        err    = enFlashAddrErr;
        goto ErrReturn;
    }

    aucTmp[i++] = aucCmd[ReadType];
    aucTmp[i++] = (ulAddr >> 16) & 0xff;
    aucTmp[i++] = (ulAddr >> 8)  & 0xff;
    aucTmp[i++] = (ulAddr >> 0)  & 0xff;
    if (enNormalRead != ReadType)
    {//fast read 多发一个dummy byte
        i++;
    }
    SpiDataRead(FLASH_SPI_CH, pucData, usDataLen, aucTmp, i);
ErrReturn:
    if (err)
    {
        PrintErr(W25Q64ReadData, 0, err);
    }
    return err;
}

void W25Q64ReadUniqID(void)
{
    uint8 ucTmp  = 0;

    ucTmp = ReadUniqID;
    SpiDataRead(FLASH_SPI_CH, aucW25Q64UniqID, sizeof(aucW25Q64UniqID), &ucTmp, 5);
    UartPrintBuffer(aucW25Q64UniqID, sizeof(aucW25Q64UniqID));
    return;
}

void W25Q64ReadSFDP(void)
{
    uint8 ucTmp[3]  = {0};
    uint8 ucSFDP    = 0; 

    ucTmp[0] = ReadSFDP;
    SpiDataRead(FLASH_SPI_CH, &ucSFDP, sizeof(ucSFDP), &ucTmp[0], 5);
    printf("SFDP0:\r\n");
    UartPrintBuffer(&ucSFDP, sizeof(ucSFDP));
    return;
}

#if FLASH_DBG
void W25Q64Test(void)
{
    uint16 i = 0;
    uint8 aucWrite[256] = {0};
    uint8 aucRead[256]  = {0};
    uint8 uctmp = 0;
		
    W25Q64Init();
    W25Q64ReadUniqID();
    W25Q64GetJEDECID();
    W25Q64ReadSFDP();
    for (i = 0; i < 256; i++)
    {
        aucWrite[i] = 256 - i ;
    }
    W25Q64PartEarse(enEarseSector, 0);
    W25Q64PagePrg(enPrgSector, 0, aucWrite, sizeof(aucWrite));
    W25Q64ReadData(enNormalRead, 0, aucRead, sizeof(aucRead));
    uctmp = memcmp(aucWrite, aucRead, sizeof(aucRead));
    printf("tmp:%d\r\n", uctmp);
    W25Q64ReadData(enFastRead, 0, aucRead, sizeof(aucRead));
    uctmp = memcmp(aucWrite, aucRead, sizeof(aucRead));
    printf("tmp:%d\r\n", uctmp);

}

#endif

#endif
