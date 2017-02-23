#ifndef __FAFS_C
#define __FAFS_C

#ifdef __cplusplus
 extern "C" {
#endif 

#include "src/diskio/diskio.c"
#include "src/ff.c"
#include "src/option/unicode.c"

#if FATFS_DBG//FATFS
void FatfsTest(uint8 *pcTmp)
{
    FATFS fs;           /* File system object */
    FIL fil;            /* File object */
    FRESULT res;        /* API result code */
    UINT bw;            /* Bytes written */
    BYTE work[_MAX_SS]; /* Work area (larger is better for process time) */


    /* Create FAT volume */
    res = f_mkfs("1:/", FM_EXFAT, 64 * 1024, work, sizeof work);
    //if (res) ...

    /* Register work area */
    res = f_mount(&fs, "1:/", 1);

    /* Create a file as new */
    res = f_open(&fil, "1:/duzailian.txt", FA_CREATE_NEW | FA_WRITE);
    //if (res) ...

    /* Write a message */
    f_write(&fil, pcTmp, strlen(pcTmp), &bw);
    //if (bw != 15) ...

    /* Close the file */
    f_close(&fil);

    /* Unregister work area */
    f_mount(0, "", 0);
}
#endif

#ifdef __cplusplus
}
#endif 
#endif
