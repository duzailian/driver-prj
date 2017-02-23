#ifndef __UART_C
#define __UART_C

PUTCHAR_PROTOTYPE
{
  UartSendByte(PRINT_PORT, (uint8_t) ch);
  return ch;
}

void UartSendByte(USART_TypeDef* USARTx, uint8 ucData)
{
    USART_SendData(USARTx, ucData);
    
    /* Loop until the end of transmission */
    while (USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET)
    {}
    return;
}

/*高位在前*/
void PrintInBinaryByte(uint8 ucData)
{
    uint8 i = 8;
    uint8 ucTmp = 0x80;

    while(i--)
    {
        UartSendByte(PRINT_PORT, (ucData & ucTmp) ? '1' : '0');
        ucTmp >>= 1;
    }
    
}

/*高字节在前，高位在前*/
void PrintInBinary(uint8 *pucData, uint8 ucLen)
{
    while(ucLen--)
    {
        PrintInBinaryByte(pucData[ucLen]);
    }
    return;
}

/*BCD print*/
void PrintInBCD(uint8 ucData)
{
    UartSendByte(PRINT_PORT, (ucData >> 4) + '0');
    UartSendByte(PRINT_PORT, (ucData & 0x0f) + '0');
    return ;
}

void UartInit(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};    
    USART_InitTypeDef USART_InitStruct = {0};

    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_Cmd(USART1, ENABLE);
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);
}

void UartPrintBuffer(uint8 *pData, uint16 usDataLen)
{
    uint16 i = 0;
    
    while(usDataLen--)
    {
        printf("0x%02x ", pData[i]);
        i++;
        if ((i % 10) == 0)
        {
            printf("\r\n");
        }
    }
    return;
}

#endif
