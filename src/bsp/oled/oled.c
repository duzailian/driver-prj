#ifndef __OLED_C
#define __OLED_C

#ifdef __cplusplus
 extern "C" {
#endif 

#include "oled_cmd.h"

#define OLED_BUFF_SZ        (128 * 64 / 8) //缓存足够放下128 X 64

/*SPI Para*/
#define OLED_SPI_CH         Spi1
#define OLED_BR            (SPI_BaudRatePrescaler_8)//spi 72M / 8

/*OLED GPIO*/
#define OLED_RST_PORT        (GPIOC)
#define OLED_RST_PIN         (GpioPin0)
#define OLED_RST_ENR         RCC_APB2EN(GPIOC)

#define OLED_DC_PORT         (GPIOC)
#define OLED_DC_PIN          (GpioPin1)
#define OLED_DC_ENR           RCC_APB2EN(GPIOC)


#define OLED_SET_RST()       do{GpioSetBit(OLED_RST_PORT, OLED_RST_PIN);}while(0)
#define OLED_CLR_RST()       do{GpioClrBit(OLED_RST_PORT, OLED_RST_PIN);}while(0)


/*汉字显示类型*/
typedef enum{
    Hz16X16,
    Hz8X8,
}OLED_HzTypeDef;

#define HZ_DATA_SZ      (16 * 16 / 8)

void OLED_SendCmd1B(uint8 ucData);

#define OLED_SendCmd2B(pData)       do{OLED_SendDC(enCmd, pData, 2);}while(0)
#define OLED_SendCmd3B(pData)       do{OLED_SendDC(enCmd, pData, 3);}while(0)
#define OLED_WriteData(pData, DataLen)  do{OLED_SendDC(enData, pData, DataLen);}while(0)

/*cmd send*/
#define OledSetContrast(level)      do{\
                                            uint8 aucTmp[2] = {ContrastSet, level};\
                                            OLED_SendCmd2B(aucTmp);\
                                        }while(0)
/*Entire Display  0:ON, 1:OFF*/
#define OledEntireDisp(on)          do{OLED_SendCmd1B(on | EntireDisp);}while(0)

/*Inverse Display 0:Normal, 1:Inverse*/
#define OledInverseDisp(on)          do{OLED_SendCmd1B(on | NormalDisp);}while(0)

/*Display  0:OFF, 1:ON*/
#define OledDispOn(on)          do{OLED_SendCmd1B(on | DispOffon);}while(0)

/*Scroll  0:OFF, 1:ON*/
#define OledEnScroll(on)          do{OLED_SendCmd1B(on | EnScroll);}while(0)

/*  Set Memory 
    Addressing Mode enOledAddrModeTypeDef*/
#define OledSetAddrMode(mode)      do{\
                                            uint8 aucTmp[2] = {0};\
                                            aucTmp[0] = SetMemAddrMode;\
                                            aucTmp[1] = mode;\
                                            OLED_SendCmd2B(aucTmp);\
                                        }while(0)
/*  Set Column Address */
#define OledSetColAddr(start, end)      do{\
                                                uint8 aucTmp[3] = {0};\
                                                aucTmp[0] = SetColAddr;\
                                                aucTmp[1] = start;\
                                                aucTmp[2] = end;\
                                                OLED_SendCmd3B(aucTmp);\
                                            }while(0)
/*
This command is only for  horizontal or vertical 
addressing mode.  
*/
/*  Set Page Address  range : 0-7*/
#define OledSetPageAddr(start, end)      do{\
                                                    uint8 aucTmp[3] = {0};\
                                                    aucTmp[0] = SetPageAddr;\
                                                    aucTmp[1] = start;\
                                                    aucTmp[2] = end;\
                                                    OLED_SendCmd3B(aucTmp);\
                                                }while(0)

/*Set Page Start Address range : 0-7*/
#define OledSetStartAddr(page)             do{OLED_SendCmd1B(page | SetPageStartAddr);}while(0)

/*Enable Segment Re-map  0:diasble, 1:enable*/
#define OledEnSegRemap(on)                 do{OLED_SendCmd1B(on | SetSegRemap);}while(0)


/*发送data or cmd*/
void OLED_SendDC(enCmdDatDef dc, uint8 *pucData, uint16 usLen)
{    
    if (enData == dc)
    {
        GpioSetBit(OLED_DC_PORT, OLED_DC_PIN);
    }
    else
    {
        GpioClrBit(OLED_DC_PORT, OLED_DC_PIN);
    }
    SpiDataWrite(OLED_SPI_CH, pucData, usLen);
    return;
}

void OLED_SendCmd1B(uint8 ucData)
{
    OLED_SendDC(enCmd, &ucData, sizeof(ucData));
    return;
}

/*Horizontal Scroll set
*/
void OLED_HorzScroll(enOledHorzDirDef rl,  enOledPageDef StartPage, enOledPageDef EndPage, enOledSpeedDef Speed)
{
    uint8 aucTmp[6] = {0};
    uint8 i =0;

    aucTmp[i++] = ScrollH | rl;
    aucTmp[i++] = DUMMY_BYTE;
    aucTmp[i++] = StartPage;
    aucTmp[i++] = Speed;
    aucTmp[i++] = EndPage;
    aucTmp[i++] = DUMMY_BYTE;
    aucTmp[i++] = 0xff;
    OLED_SendDC(enCmd, aucTmp, sizeof(aucTmp));
    return;
}
/*设置垂直且水平滚动
*/
void OLED_VertScroll(enOledVertDirDef rl,  enOledPageDef StartPage, enOledPageDef EndPage, enOledSpeedDef Speed)
{
    uint8 aucTmp[6] = {0};
    uint8 i =0;

    aucTmp[i++] = ScrollV | rl;
    aucTmp[i++] = DUMMY_BYTE;
    aucTmp[i++] = StartPage;
    aucTmp[i++] = Speed;
    aucTmp[i++] = EndPage;
    aucTmp[i++] = 0xff;
    
    OLED_SendDC(enCmd, aucTmp, sizeof(aucTmp));
    return;
}
/*
    设置水平滚动的区域范围
    FixRows :屏幕上端固定显示的rows数[0-0x3f]
    ScrollRows 滚动显示的总rows数[0-0x7f]
*/
void OLED_VertScrollArea(enOledVertDirDef rl,  uint8 FixRows, uint8 ScrollRows)
{
    uint8 aucTmp[3] = {0};
    uint8 i =0;

    aucTmp[i++] = VScrollArea;
    aucTmp[i++] = FixRows;
    aucTmp[i++] = ScrollRows;
    
    OLED_SendCmd3B(aucTmp);
    return;
}

/* (Page Addressing Mode 专用)
    Set Start Address for Page Addressing Mode 
*/
void OLED_SetSartAddr(uint8 ucStartAddr)
{    
    uint8 i =0;
    uint8 aucTmp[2] = {0};
		
    aucTmp[i++] = SetCctartAddrL | (ucStartAddr && 0xff);
    aucTmp[i++] = SetCctartAddrH | ((ucStartAddr >> 4) && 0xff);
    
    OLED_SendCmd2B(aucTmp);
    return;
}
void OLED_PowerOn(void)
{
    /*rest*/
    OLED_CLR_RST();
    DelayUs(10);
    OLED_SET_RST();
    
    OledSetAddrMode(HorzMode);
    OledDispOn(ON);
    
    return;
}

void OLED_GpioInit(void)
{
    GpioInitDef  GPIO_InitStruct;

    //Rst Pin
    RCC->APB2ENR |= OLED_RST_ENR;

    GPIO_InitStruct.GpioPin    = OLED_RST_PIN;
    GPIO_InitStruct.GPIO_Mode  = GpioModeOutPP;
    GPIO_InitStruct.GPIO_Speed = GpioSpeed50MHz;
    GpioInit(OLED_RST_PORT, &GPIO_InitStruct);

    /*DC Pin*/
    RCC->APB2ENR |= OLED_DC_ENR;

    GPIO_InitStruct.GpioPin    = OLED_DC_PIN;
    GPIO_InitStruct.GPIO_Mode  = GpioModeOutPP;
    GPIO_InitStruct.GPIO_Speed = GpioSpeed50MHz;
    GpioInit(OLED_DC_PORT, &GPIO_InitStruct);
    return;
}

void OLED_Init(void)
{    
    /*init oled gpio*/
    OLED_GpioInit();
    /*spi init*/
    {
        stSpiInitDef stSpiInitInfo;
        SPI_InitTypeDef SPI_InitStruct;//used in StdPeriph_Lib

        /*Spi Init*/
        SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
        SPI_InitStruct.SPI_Mode      = SPI_Mode_Master;
        SPI_InitStruct.SPI_DataSize  = SPI_DataSize_8b;
        SPI_InitStruct.SPI_CPOL      = SPI_CPOL_Low;
        SPI_InitStruct.SPI_CPHA      = SPI_CPHA_1Edge;        
        SPI_InitStruct.SPI_NSS       = SPI_NSS_Soft;
        SPI_InitStruct.SPI_BaudRatePrescaler  = OLED_BR;
        SPI_InitStruct.SPI_FirstBit  = SPI_FirstBit_MSB;
        SPI_InitStruct.SPI_CRCPolynomial  = 1;

        stSpiInitInfo.Ch        = OLED_SPI_CH;
        stSpiInitInfo.SPI_InitStruct = &SPI_InitStruct;
        SpiInit(&stSpiInitInfo);
    }

    OLED_PowerOn();
    return;
}

/*显示一个16X16汉字
0---------X----->
|
|
Y
|
|
V
para:    x,y 汉字显示起始坐标 x(0-112), y(OLED_Y0-OLED_Y48)

pData:   
*/
enErrDef OLED_DispHz16X16(uint8 x, OLED_ValidYDef y, uint8 *pData)
{
    enErrDef err  = enOk;
    if ((x > 112) || (y > OLED_Y48))
    {
        err = enOledParaErr;
        goto ErrReturn;
    }
    OledSetColAddr(x, x + 15);
    OledSetPageAddr(y, y + 1);
    OLED_WriteData(pData, HZ_DATA_SZ);
    return enOk;
ErrReturn:
    PrintErr(OLED_DispHz16X16, 0, err);
    return err;
}

/*
    显示BMP图片
Para:   
(x0,y0),(x,y)分别为起始(左上)和结束(右下)的坐标
pData为指向图片数据的指针
usLen为数据长度
*/
enErrDef OLED_DispBmp(uint8 x0, OLED_ValidYDef y0, uint8 x, OLED_ValidYDef y, uint8 *pData, uint16 usLen)
{
    enErrDef err  = enOk;
    if ((x < x0) || (y < y0) || (0 != (x0 % 8)))
    {
        err  = enOledParaErr;
        goto ErrRetrun;
    }
    //OledSetColAddr(y0, y);
    //OledSetColAddr(y0, y);
    //OLED_SendDC(enData, pData, usLen);
ErrRetrun:
    return err;
}
#if OLED_DBG//OLED

static uint8 aucTmp1[] = {
0x20,0x20,0xA0,0xFE,0xA0,0x20,0xA0,0x80,0x80,0xFE,0x80,0x80,0x80,0x80,0x00,0x00,
0x08,0x06,0x01,0xFF,0x00,0x43,0x40,0x40,0x40,0x7F,0x40,0x40,0x40,0x40,0x40,0x00,/*"?",0*/
};
void OLED_Test(void)
{
    OLED_Init();
    OLED_DispHz16X16(0, OLED_Y0, &aucTmp1[0]);
}
#endif

#ifdef __cplusplus
}
#endif
#endif
