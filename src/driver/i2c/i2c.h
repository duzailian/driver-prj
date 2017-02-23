#ifndef __I2C_H
#define __I2C_H

#ifdef __cplusplus
 extern "C" {
#endif 

typedef enum{    
    I2cCh0  = 0,
    I2cCh1,
    I2cCh2,
    I2cCh3,
}enI2cChDef;

typedef const struct{    
    /*SCL*/
    stGpioDef    GpioSCL;//I2c scl pin info
    /*SDA*/
    stGpioDef    GpioSDA;//I2c sda pin info
}stI2cGpioDef;

typedef struct{    
    /*channel*/
    enI2cChDef   ch;
    enDataDirDef   rw;
    uint8        SlaveAddr;
    uint16       usLen;
}stI2cTranDef;

extern void I2cInit(enI2cChDef ch);
extern uint16 I2cWrite(enI2cChDef ch, uint8 SlaveAddr, uint8 *pucData, uint16 usLen);
extern void I2cRead(enI2cChDef ch, uint8 SlaveAddr, uint8 *pucData, uint16 usLen);
extern void I2cStart(enI2cChDef ch, uint8 addr, enDataDirDef rw);
extern uint16 I2cWriteByte(enI2cChDef ch, uint8 ucData);

#ifdef __cplusplus
}
#endif
#endif

