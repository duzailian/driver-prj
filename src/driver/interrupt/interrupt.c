#ifndef __INTERRUPT_C
#define __INTERRUPT_C
#ifdef __cplusplus
 extern "C" {
#endif 

#define NVIC_PRIO_GROUP         5

#define PrintIntErr(Func)          printf("Irq %s Config Err!\r\n", #Func);

#define IRQHandler(irq)             void irq##_IRQHandler(void)\
                                     {\
                                        if (apIrqFunc[irq##_IRQn])\
                                        {\
                                            apIrqFunc[irq##_IRQn]();\
                                        }\
                                        else\
                                        {\
                                            PrintIntErr(irq##_IRQHandler);\
                                        }\
                                        return;\
                                    }

typedef const struct{
IRQn_Type IRQn;
uint32_t PreemptPriority;//抢占优先级
uint32_t SubPriority;
}stIRQnInfoDef;

static pIrqFunc apIrqFunc[IRQnMax] = {NULL,};
static stIRQnInfoDef astIRQnInfo[] ={

    {   /*Rc522 Interrupt Number*/  /*抢占优先级*/      /*响应优先级*/
        RC522_INT_IRQn,             0,                  5,
    }
};

IRQHandler(WWDG)
IRQHandler(PVD)
IRQHandler(TAMPER)
IRQHandler(RTC)
IRQHandler(FLASH)
IRQHandler(RCC)
IRQHandler(EXTI0)
IRQHandler(EXTI1)
IRQHandler(EXTI2)
IRQHandler(EXTI3)
IRQHandler(EXTI4)
IRQHandler(DMA1_Channel1)
IRQHandler(DMA1_Channel2)
IRQHandler(DMA1_Channel3)
IRQHandler(DMA1_Channel4)
IRQHandler(DMA1_Channel5)
IRQHandler(DMA1_Channel6)
IRQHandler(DMA1_Channel7)
IRQHandler(ADC1_2)
IRQHandler(USB_HP_CAN1_TX)
IRQHandler(USB_LP_CAN1_RX0)
IRQHandler(CAN1_RX1)
IRQHandler(CAN1_SCE)
IRQHandler(EXTI9_5)
IRQHandler(TIM1_BRK)
IRQHandler(TIM1_UP)
IRQHandler(TIM1_TRG_COM)
IRQHandler(TIM1_CC)
IRQHandler(TIM2)
IRQHandler(TIM3)
IRQHandler(TIM4)
IRQHandler(I2C1_EV)
IRQHandler(I2C1_ER)
IRQHandler(I2C2_EV)
IRQHandler(I2C2_ER)
IRQHandler(SPI1)
IRQHandler(SPI2)
IRQHandler(USART1)
IRQHandler(USART2)
IRQHandler(USART3)
IRQHandler(EXTI15_10)
IRQHandler(RTCAlarm)
IRQHandler(USBWakeUp)
IRQHandler(TIM8_BRK)
IRQHandler(TIM8_UP)
IRQHandler(TIM8_TRG_COM)
IRQHandler(TIM8_CC)
IRQHandler(ADC3)
IRQHandler(FSMC)
IRQHandler(SDIO)
IRQHandler(TIM5)
IRQHandler(SPI3)
IRQHandler(UART4)
IRQHandler(UART5)
IRQHandler(TIM6)
IRQHandler(TIM7)
IRQHandler(DMA2_Channel1)
IRQHandler(DMA2_Channel2)
IRQHandler(DMA2_Channel3)
IRQHandler(DMA2_Channel4_5)

void IrqRegester(IRQn_Type IRQn, pIrqFunc IrqFunc)
{
    apIrqFunc[IRQn] = IrqFunc;
    return;
}

void InterruptCfg(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
    uint32_t ulPriority = 0;
    
    ulPriority = NVIC_EncodePriority(NVIC_PRIO_GROUP, PreemptPriority, SubPriority);
    NVIC_SetPriority(IRQn, ulPriority);
    NVIC_EnableIRQ(IRQn);
    return;
}

void InterruptInit(void)
{
    uint8 i = 0;
    
    NVIC_SetPriorityGrouping(NVIC_PRIO_GROUP);
    for (i = 0; i < SizeOfArray(astIRQnInfo); i++)
    {
        InterruptCfg(astIRQnInfo[i].IRQn, astIRQnInfo[i].PreemptPriority, astIRQnInfo[i].SubPriority);
    }
    return;
}

#ifdef __cplusplus
}
#endif
#endif
