#ifndef __MIDDLE_CFG_H
#define __MIDDLE_CFG_H

/* config */
#define FATFS_EN			0

#if FATFS_EN
/* config file */
#include "fatfs/src/ffconf.h"
#endif

#endif
