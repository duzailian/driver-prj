#ifndef __BSP_C
#define __BSP_C

#include "bsp_cfg.h"
#include "bsp.h"
#include "driver.h"

#if OLED_EN
#include "oled/oled.c"
#endif

#if E2PROM_EN
#include "E2PROM/E2PROM.C"
#endif

#if SD_CARD_EN
#include "sdcard/sdcard.c"
#endif

#if RFID_EN
#include "rfid/rfid.c"
#endif

#if W25Q64_EN
#include "w25q64/w25q64.c"
#endif

#if BEEP_EN
#include "beep/beep.c"
#endif

void BspInit(void)
{
#if OLED_EN
    OLED_Init();
#endif
    
#if E2PROM_EN//
    E2PROM_Init();
#endif
    
#if SD_CARD_EN
    SdCardInit();
#endif
    
#if RFID_EN//RFID初始化需要用到外挂的E2PROM,需要在E2PROM_Init()后
    RFIDInit();
#endif
    
#if W25Q64_EN
    W25Q64Init();
#endif

#if BEEP_EN
    BeepInit();
#endif
    return;
}

#endif
