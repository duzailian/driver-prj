#ifndef   DISKIO_SD_C
#define 	DISKIO_SD_C

#ifdef __cplusplus
 extern "C" {
#endif 
	 
/*FOR FATFS*/
DRESULT SD_EarseBlock(DWORD pulBlockAddr)
{
    DRESULT err = RES_OK;

    if (SdMultiBlockEarse(pulBlockAddr))
    {
        err = RES_ERROR;
    }
    else
    {
        err = RES_OK;
    }
    return err;
}

DSTATUS SD_disk_status (void)
{
    DSTATUS ucTmp = 0;
    
    if(ucSdStaFlg & SD_NOINIT) 
    {
        ucTmp |= STA_NOINIT;
    }

    if(ucSdStaFlg & SD_PROTECT) 
    {
        ucTmp |= STA_PROTECT;
    }
    return ucTmp;
}

DSTATUS SD_disk_initialize (void)
{
    DSTATUS res = SD_disk_status();

    if (res & STA_NOINIT)
    {
        SdCardInit();
        res = SD_disk_status();
    }
    return res;
}

/*
RES_OK (0)
The function succeeded.
RES_ERROR
A hard error occured during the read operation and could not recover it.
RES_PARERR
Invalid parameter.
RES_NOTRDY
The device has not been initialized.
*/    
static __align(4) uint8 aucTmp[SD_DATA_BLOCK_SZ * 2] = {0};
DRESULT SD_disk_read (
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    DRESULT err = RES_OK;

    if ((NULL == buff) || ((sector + count)> ulSdMaxSector) || (count > 2))
    {
        err = RES_PARERR;
        goto ErrReturn;
    }
    if (ucSdStaFlg & SD_NOINIT)
    {
        err = RES_NOTRDY;
        goto ErrReturn;
    }
    if (0 == SdMultiBlockRead(aucTmp, sector, count))
    {
        memcpy(buff, aucTmp, count * SD_DATA_BLOCK_SZ);
        err = RES_OK;
    }
    else
    {
        err = RES_ERROR;
        goto ErrReturn;
    }
    
ErrReturn:
    return err;
}

/*
RES_OK (0)
The function succeeded.
RES_ERROR
A hard error occured during the write operation and could not recover it.
RES_WRPRT
The medium is write protected.
RES_PARERR
Invalid parameter.
RES_NOTRDY
The device has not been initialized.
*/
DRESULT SD_disk_write (
	const BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Start sector in LBA */
	UINT count		/* Number of sectors to read */
)
{
    DRESULT err = RES_OK;

    if ((NULL == buff) || ((sector + count)> ulSdMaxSector))
    {
        err = RES_PARERR;
        goto ErrReturn;
    }

    if (ucSdStaFlg & SD_NOINIT)
    {
        err = RES_NOTRDY;
        goto ErrReturn;
    }
    if (ucSdStaFlg & SD_PROTECT)
    {
        err = RES_WRPRT;
        goto ErrReturn;
    }
    if (0 == SdMultiBlockWrite(buff, sector, count))
    {
        err = RES_OK;
    }
    else
    {
        err = RES_ERROR;
        goto ErrReturn;
    }
    
ErrReturn:
    return err;
}

DRESULT SD_CtrlSync(void)
{
    DRESULT err = RES_OK;

    return err;
}

DRESULT SD_disk_ioctl (
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    DRESULT err = RES_OK;

    switch(cmd)
    {
        case CTRL_SYNC:
        {
            err = SD_CtrlSync();
            break;
        }
        case GET_SECTOR_COUNT:
        {
            err = RES_OK;
            *(DWORD*)buff = ulSdMaxSector;
            break;
        }
        case GET_SECTOR_SIZE:
        {
            break;
        }
        case GET_BLOCK_SIZE:
        {
            err = RES_OK;
            *(DWORD*)buff = SD_BLOCK_SIZE;
            break;
        }
        case CTRL_TRIM:
        {
            err = SD_EarseBlock(*(DWORD *)buff);
            break;
        }
        default:
        {
            break;
        }
    }
    
    return err;
}


#ifdef __cplusplus
}
#endif


#endif /* DISKIO_SD_C */
