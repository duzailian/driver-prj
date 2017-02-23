#ifndef __GPIO_H
#define __GPIO_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define HIGH                         1
#define LOW                          0

/*重写gpio init函数，以方便bitband使用*/
typedef enum
{ 
    GpioModeAin = 0x0,
    GpioModeInFloat = 0x04,
    GpioModeIPD = 0x08,
    GpioModeIPU = 0x18,
    GpioModeOutPP = 0x10,
    GpioModeOutOD = 0x14,
    GpioModeAfpp = 0x18,
    GpioModeAfOD = 0x1C,
}GpioModeDef;

typedef enum
{ 
    GpioInput = 0,
    GpioSpeed10MHz,
    GpioSpeed2MHz, 
    GpioSpeed50MHz
}GpioSpeedDef;

typedef enum{    
    GpioOut,
    GpioIn,
}GpioDirDef;

typedef enum
{ 
    GpioPin0,
    GpioPin1,
    GpioPin2,
    GpioPin3,
    GpioPin4,
    GpioPin5,
    GpioPin6,
    GpioPin7,
    GpioPin8,    
    GpioPin9,
    GpioPin10,
    GpioPin11,
    GpioPin12,
    GpioPin13,
    GpioPin14,
    GpioPin15,
}GpioPinDef;

typedef struct
{
    GpioPinDef GpioPin;            //0~15
    GpioSpeedDef GPIO_Speed;  
    GpioModeDef GPIO_Mode;   
}GpioInitDef;

typedef const struct{
    GPIO_TypeDef*   GpioPort;
    GpioPinDef  GpioPin;//0~15
    uint32_t        RccEn;//RCC_APB2Periph_SPI1 etc.
}stGpioDef;

void GpioInit(GPIO_TypeDef* GPIOx, GpioInitDef* GPIO_InitStruct);
#define GpioSetBit(port, bit)       do{BITBAND(&(port->ODR), bit) = 1;}while(0)
#define GpioClrBit(port, bit)       do{BITBAND(&(port->ODR), bit) = 0;}while(0)
#define GpioReadBit(port, bit)      BITBAND(&(port->IDR), bit)
#define GpioReadBitO(port, bit)     BITBAND(&(port->ODR), bit)

#ifdef __cplusplus
}
#endif
#endif
