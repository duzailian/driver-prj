#ifndef __SDCARD_H
#define __SDCARD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define SD_DBG          0

#define SD_NOINIT       0x01
#define SD_PROTECT      0x02
	 
#define SD_DATA_BLOCK_SZ        512//block size
#define SD_BLOCK_SIZE          (2)//Earse block size 

extern void SdCardInit(void);
extern void SdBlockRead(uint32 ulBlockNum);
extern void SdBlockWrite(uint32 ulBlockNum);

extern uint16 SdMultiBlockWrite(
	const uint8 *buff,		/* Data buffer to store read data */
	uint32 sector,	/* Start sector in LBA */
	uint32 count		/* Number of sectors to read */
);
extern uint16 SdMultiBlockRead(
	uint8 *buff,		/* Data buffer to store read data */
	uint32 sector,	/* Start sector in LBA */
	uint32 count		/* Number of sectors to read */
);
extern uint16 SdMultiBlockEarse(
	uint32 sector	/* Start sector in LBA */
);


extern uint32 ulSdMaxSector;
extern uint8 ucSdStaFlg;
#if SD_DBG
extern void SdTest(void);
#endif

#ifdef __cplusplus
}
#endif


#endif /* __SDIO_H */

