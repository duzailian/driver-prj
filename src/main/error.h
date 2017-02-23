#ifndef __ERROR_H
#define __ERROR_H

#ifdef __cplusplus
 extern "C" {
#endif 

/*
err index:
    1-50: stm32 Peripheral Err,
    50-150: bsp Err,
*/
typedef enum{
enOk = 0,
/*    0-50: stm32 Peripheral Err,*/

/*    50-150: bsp Err,*/
enFlashRErr = 50,//w25q64Err
enFlashWErr,//w25q64Err

enFlashLenErr,//w25q64 data len Err
enFlashAddrErr,//w25q64 addr len Err

enOledParaErr,//oled para Err

}enErrDef;

#ifdef __cplusplus
}
#endif
#endif

