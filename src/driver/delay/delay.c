#ifndef __DELAY_C
#define __DELAY_C

#define RTC_PRL         1    //预分频器设置值

void DelayInit(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    RCC->BDCR |= 1;//LSE on
    while(!(RCC->BDCR & (1 << 1)));//wait for ready
    RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
    
    RCC_RTCCLKCmd(ENABLE);

    /*RTC CFG*/
    RTC_WaitForLastTask();

    RTC_SetPrescaler(RTC_PRL);// 1/16ms

    RTC_WaitForLastTask();
    PWR_BackupAccessCmd(DISABLE);    

    return;
}
/*
注意:
    1.usMs最大值为0xffff * (RTC_PRL + 1) / 32
*/
void DelayMs(uint16 usMs)
{
    uint16 usTmp = 0;

    usMs *= (32 / (RTC_PRL + 1));
    usTmp = (RTC_GetCounter() & 0xffff) + usMs;

    while(usTmp != (RTC_GetCounter() & 0xffff));
    return;
}

void DelayUs(uint16 usUs)
{
    SysTick->LOAD  = usUs * 72;
    SysTick->CTRL = SysTick_CTRL_ENABLE_Msk | 	SysTick_CTRL_CLKSOURCE_Msk;

    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk));
	
	  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk ;
}

#if DELAY_DBG

void DelayTestGpioInit(void)
{
    GpioInitDef  GPIO_InitStruct;

    //Rst Pin
    RCC->APB2ENR |= RCC_APB2EN(GPIOA) | RCC_APB2EN(GPIOB);

    GPIO_InitStruct.GpioPin    = GpioPin0;
    GPIO_InitStruct.GPIO_Mode  = GpioModeOutPP;
    GPIO_InitStruct.GPIO_Speed = GpioSpeed50MHz;
    GpioInit(GPIOA, &GPIO_InitStruct);
    return;
}

void DelayTest(void)
{
    DelayTestGpioInit();
    while(1)
    {
        GpioClrBit(GPIOA, GpioPin0);
        DelayUs(5);
        GpioSetBit(GPIOA, GpioPin0);
        DelayUs(10);
    }
    return;
}
#endif

#endif

