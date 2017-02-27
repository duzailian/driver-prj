#ifndef __BSP_H
#define __BSP_H

#include "bsp_cfg.h"
#include "error.h"

#include "lib.h"
#include "common.h"

#if OLED_EN
#include "oled/oled.h"
#endif

#if E2PROM_EN
#include "E2PROM/E2PROM.h"
#endif

#if SD_CARD_EN
#include "sdcard/sdcard.h"
#endif

#if RFID_EN
#include "rfid/rfid.H"
#endif

#if W25Q64_EN
#include "w25q64/w25q64.h"
#endif

#if BEEP_EN
#include "beep/beep.h"
#endif

#if NRF_EN
#include "24l01/nrf24l01.h"
#endif

extern void BspInit(void);

#endif
