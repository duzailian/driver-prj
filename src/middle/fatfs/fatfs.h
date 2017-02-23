#ifndef __FAFS_H
#define __FAFS_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define FATFS_DBG       0

#include "src/integer.h"
#include "src/diskio/diskio.h"
#include "src/ffconf.h"
#include "src/ff.h"

#if FATFS_DBG//FATFS
extern void FatfsTest(uint8 *pcTmp);
#endif

#ifdef __cplusplus
}
#endif 
#endif
