#ifndef __NRF_CFG_H
#define __NRF_CFG_H

#ifdef __cplusplus
 extern "C" {
#endif 

/***************SPI Cfg****************/
#define NRF_SPI_CH           (Spi2)
/*SPI Baud rate*/
#define NRF_SPI_BR           (SPI_BaudRatePrescaler_8)//spi 72M / 8

/**************Gpio Connect*************/
#define NRF_CE_PORT          GPIOG
#define NRF_CE_PIN           GpioPin6
#define NRF_CE_ENR           RCC_APB2EN(GPIOG)

#define NRF_INT_PORT         GPIOG
#define NRF_INT_PIN          GpioPin8
#define NRF_INT_ENR          RCC_APB2EN(GPIOG)
#define NRF_INT_LINE         (EXTI_Line8)

/********NRF24L01 Register Cfg**********/
#define NRF_INT_MASK         //Mask interrupt 


#ifdef __cplusplus
}
#endif
#endif//__NRF_CFG_H


