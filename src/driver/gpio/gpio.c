#ifndef __GPIO_C
#define __GPIO_C

#ifdef __cplusplus
 extern "C" {
#endif 

/*
功能:
    根据GPIO_InitStruct对GPIOx进行配置
参数:
    GPIO_InitStruct: 存储配置的结构体
    GPIOx: 要配置的端口，如GPIOA
返回值: void
输入:   void
输出:   void
备注:   void
*/
void GpioInit(GPIO_TypeDef* GPIOx, GpioInitDef* GPIO_InitStruct)
{
    __IO uint32_t *CR = &(GPIOx->CRL);
    uint8 pin = GPIO_InitStruct->GpioPin;
    uint32 ulTmp = GPIO_InitStruct->GPIO_Speed | GPIO_InitStruct->GPIO_Mode & 0x0f;

    /*Input with pull-up / pull-down*/
    if (GpioInput == GPIO_InitStruct->GPIO_Speed)
    {
        if (GpioModeIPU == GPIO_InitStruct->GPIO_Mode)
        {
            GpioSetBit(GPIOx, pin);
        }
        else if (GpioModeIPD == GPIO_InitStruct->GPIO_Mode)
        {
            GpioClrBit(GPIOx, pin);
        }
    }
    if (pin > 7)
    {
        CR = &(GPIOx->CRH);
        pin %= 8;
    }
    ulTmp <<= (4 * pin);
    *CR &= ~(0x0f << (4 * pin));
    *CR |= ulTmp;
    return;
}

#ifdef __cplusplus
}
#endif
#endif
