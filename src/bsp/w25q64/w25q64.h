#ifndef __W25Q64_H
#define __W25Q64_H

#define     FLASH_DBG       0

typedef enum{
    enEarseSector,
    enEarseBlock32k,
    enEarseBlock64k,
    enEarseSecurity,//Erase Security Registers
}enW25Q64EarseType;

typedef enum{
    enPrgSector,
    enPrgSecurity,//Prg Security Registers
}enW25Q64PrgType;

typedef enum{
    enNormalRead,
    enFastRead,
    enSecurityRead,
}enW25Q64ReadType;

extern enErrDef W25Q64Init(void);
extern enErrDef W25Q64GetJEDECID(void);
extern void W25Q64ReadSFDP(void);
extern void W25Q64ReadUniqID(void);
extern enErrDef W25Q64PagePrg(enW25Q64PrgType PrgType, uint32 ulAddr, uint8 const *const pucData, uint16 const usDataLen);
extern enErrDef W25Q64ReadData(enW25Q64ReadType ReadType, uint32 ulAddr, uint8 * const pucData, uint16 usDataLen);
extern enErrDef W25Q64PartEarse(enW25Q64EarseType  EarseType, uint32 ulAddr);
#if FLASH_DBG
extern void W25Q64Test(void);
#endif

#endif
