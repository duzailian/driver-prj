#ifndef __MIDDLE_C
#define __MIDDLE_C

#ifdef __cplusplus
 extern "C" {
#endif 

#include "cfg.h"

/***************************±ØÐë*************************************/
#include "driver.h"
#include "bsp.h"
#include "middle.h"


/****************************optional********************************/
#if FATFS_EN
#include "fatfs/fatfs.c"
#endif

#if E2PROM_EN
#include "E2PROM_MGR/E2PROM_MGR.c"
#endif

void MiddleInit(void)
{
    return;
}

#ifdef __cplusplus
}
#endif 
#endif
