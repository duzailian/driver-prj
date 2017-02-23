#ifndef __MIDDLE_H
#define __MIDDLE_H

#ifdef __cplusplus
 extern "C" {
#endif 

/*************************Í·ÎÄ¼þ********************************/
#include "cfg.h"

#if FATFS_EN
#include "fatfs/fatfs.h"
#endif

#if E2PROM_EN
#include "E2PROM_MGR/E2PROM_MGR.h"
#endif

extern void MiddleInit(void);


#ifdef __cplusplus
}
#endif 
#endif
