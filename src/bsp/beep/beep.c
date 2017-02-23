#ifndef __BEEP_C
#define __BEEP_C

#ifdef __cplusplus
 extern "C" {
#endif 

/*
功能:
    1.蜂鸣器初始化
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
*/
void BeepInit(void)
{
    GpioInitDef GPIO_InitStruct;
    
    RCC->APB2ENR |= BEEP_ENR;

    GPIO_InitStruct.GpioPin    = BEEP_PIN;
    GPIO_InitStruct.GPIO_Mode  = GpioModeOutPP;
    GPIO_InitStruct.GPIO_Speed = GpioSpeed50MHz;
    GpioInit(BEEP_PORT, &GPIO_InitStruct);
    return;
}

/*
功能:
    1.蜂鸣器单次发声(短音/长音)
参数:   
    1.enBpType :BeepShort(短音)2.BeepLong(长音)
返回值: void
输入:   void
输出:   void
备注:   void
*/
static void BeepS(enBpTypedef enBpType)
{
    uint16 usDelay = 0;

    if (BeepShort == enBpType)
    {
        usDelay = BEEP_SHORT_T;
    }
    else
    {
        usDelay = BEEP_LONG_T;
    }
    GpioSetBit(BEEP_PORT, BEEP_PIN);
    DelayMs(usDelay);
    GpioClrBit(BEEP_PORT, BEEP_PIN);
    return;
}

/*
功能:
    1.蜂鸣器发双音
参数:   void
返回值: void
输入:   void
输出:   void
备注:   void
*/
static void BeepD(void)
{
    uint8 i = 0;

    for(i = 0; i < 2; i++)
    {
        BeepS(BeepShort);
        DelayMs(BEEP_DOUBLE_T);
    }
    return;
}

/*
功能:
    1.蜂鸣器发声
参数:   
    enBpType :1.BeepShort(短音)2.BeepLong(长音)3.BeepDouble(双响)
返回值: void
输入:   void
输出:   void
备注:   void
*/
void Beep(enBpTypedef enBpType)
{
    switch(enBpType)
    {
        case BeepShort:
        case BeepLong:
        {
            BeepS(enBpType);
            break;
        }
        case BeepDouble:
        {
            BeepD();
            break;
        }
        default:
        {
            break;
        }
    }
    return;
}

#if BEEP_DBG
void BeepTest(void)
{
    BeepInit();
    while(1)
    {
        Beep(BeepShort);
        DelayMs(2000);
        Beep(BeepLong);
        DelayMs(2000);
        Beep(BeepDouble);
        DelayMs(2000);/**/
    }
    return;
}
#endif

#ifdef __cplusplus
}
#endif
#endif /* __BEEP_C */

