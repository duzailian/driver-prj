#ifndef __OLED_CMD_H
#define __OLED_CMD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define DUMMY_BYTE          0

typedef enum{
    PAGE0,
    PAGE1,
    PAGE2,
    PAGE3,
    PAGE4,
    PAGE5,
    PAGE6,
    PAGE7,
}enOledPageDef;

/*水平Scroll方向*/
typedef enum{
    enRight,
    enLeft,
}enOledHorzDirDef;

/*水平Scroll方向*/
typedef enum{
    enVRight = 0x01,// Vertical and Right Horizontal Scroll
    enVLeft  = 0x02,// Vertical and Left Horizontal Scroll
}enOledVertDirDef;

/*Scroll Speed Level*/
typedef enum{
    enSpeed0,
    enSpeed1,
    enSpeed2,
    enSpeed3,
    enSpeed4,
    enSpeed5,
    enSpeed6,
    enSpeed7,
}enOledSpeedDef;

/*Set Memory Addressing Mode */
typedef enum{
    HorzMode,
    VertMode,
    PageMode
}enOledAddrModeDef;

/*Fundamental Command Table */
#define     ContrastSet     (0x81)//+1Byte level(1--256)
#define     EntireDisp      (0xa4)//=0:normal(reset) =1:Entire
#define     NormalDisp      (0xa6)//=0:normal(reset) =1:Inverse
#define     DispOffon       (0xae)//=0:off(reset)    =1:on

/*Scrolling Command Table */
#define     ScrollH       (0x26)//Horizontal Scroll  =0:Right =1:Left
#define     ScrollV       (0x28)/*Vertical Scroll 
                                 =01b : Vertical and Right Horizontal Scroll
                                 =10b : Vertical and Left Horizontal Scroll*/
#define     EnScroll        (0x2e)//enable Scroll 
#define     VScrollArea     (0xa3)//Set Vertical Scroll Area

/*Addressing Setting Command Table */
#define     SetCctartAddrL     (0x00)//Set the lower nibble of the column start address 
                                        //for Page Addressing Mode
#define     SetCctartAddrH     (0x10)//Set the lower nibble of the column start address 
                                        //for Page Addressing Mode

#define     SetMemAddrMode      (0x20)//Set Memory Addressing Mode 
#define     SetColAddr          (0x21)//Setup column start and end address 
#define     SetPageAddr         (0x22)//Setup page start and end address 
#define     SetPageStartAddr    (0xB0)//Set GDDRAM Page Start Address

/*Hardware Configuration*/
#define     SetDispStartLine    (0x40)//Set display RAM display start line register from 
#define     SetSegRemap          (0xA0)//Set Segment Re-map
#define     SetMultiRatio        (0xA8)//Set MUX ratio to N+1 MUX 
#define     SetScanDir           (0xC0)//Set COM Output Scan Direction 
#define     SetDispOffset        (0xD3)//Set vertical shift by COM from 0d~63d 
#define     SetMultiRatio        (0xA8)//Set MUX ratio to N+1 MUX 

#ifdef __cplusplus
}
#endif


#endif /* __SDIO_H */


