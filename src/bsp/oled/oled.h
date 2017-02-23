#ifndef __OLED_H
#define __OLED_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define OLED_DBG            0

/*可用的Y坐标定义*/
typedef enum{
    OLED_Y0,
    OLED_Y8,
    OLED_Y16,
    OLED_Y24,
    OLED_Y32,
    OLED_Y40,
    OLED_Y48,
    OLED_Y56,
}OLED_ValidYDef;

extern void OLED_Init(void);
extern enErrDef OLED_DispHz16X16(uint8 x, OLED_ValidYDef y, uint8 *pData);

#if OLED_DBG//OLED
extern void OLED_Test(void);
#endif


#ifdef __cplusplus
}
#endif
#endif
