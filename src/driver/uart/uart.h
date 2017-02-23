#ifndef __UART_H
#define __UART_H

#ifdef __cplusplus
 extern "C" {
#endif 
	 
#define PRINT_PORT          USART1

#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void UartInit(void);
void UartSendByte(USART_TypeDef* USARTx, uint8 ucData);
void PrintInBinaryByte(uint8 ucData);
void PrintInBinary(uint8 *pucData, uint8 ucLen);
void PrintInBCD(uint8 ucData);
void UartPrintBuffer(uint8 *pData, uint16 usDataLen);


#ifdef __cplusplus
}
#endif
#endif
