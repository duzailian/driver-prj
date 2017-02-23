#ifndef __E2PROM_H
#define __E2PROM_H

#define E2PROM_24C01        0x01
#define E2PROM_24C02        0x02
#define E2PROM_24C04        0x03
#define E2PROM_24C08        0x04
#define E2PROM_24C16        0x05

#define E2PROM_TYPE         E2PROM_24C02

#if (E2PROM_TYPE == E2PROM_24C01)
#define E2PROM_CAP         (1 * 1024 / 8)//24c02 capacity 1K
#elif (E2PROM_TYPE == E2PROM_24C02)
#define E2PROM_CAP         (2 * 1024 / 8)//24c02 capacity 2K
#elif (E2PROM_TYPE == E2PROM_24C04)
#define E2PROM_CAP         (4 * 1024 / 8)//24c02 capacity 4K
#elif (E2PROM_TYPE == E2PROM_24C08)
#define E2PROM_CAP         (8 * 1024 / 8)//24c02 capacity 8K
#elif (E2PROM_TYPE == E2PROM_24C16)
#define E2PROM_CAP         (16 * 1024 / 8)//24c02 capacity 2K
#endif
 
#define E2PROM_DBG            0

extern void E2PROM_Init(void);
extern void E2PROM_Read(uint8 ucAddr, uint8 *pucRead, uint16 ucReadSz);
extern void E2PROM_Write(uint8 ucAddr, uint8 *pucWrite, uint16 usWriteSz);
#if E2PROM_DBG
void E2PROM_Test(void);
#endif

#endif
