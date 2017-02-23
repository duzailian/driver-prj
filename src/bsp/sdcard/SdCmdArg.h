#ifndef __SD_CMD_ARG_H
#define __SD_CMD_ARG_H

#ifdef __cplusplus
 extern "C" {
#endif

/*CMD arg*/
#define SD_CMD_ARG_NULL           0x00

/*ARG for cmd8 & R7*/
#define SD_CMD_ARG_VHS             0x01// 0001b Voltage 2.7-3.6V 
#define SD_CMD_ARG_PATTERN         0xaa

/*ARG for acmd41*/
#define SD_CMD_ARG_HCS              (1 << 30)//High Capacity Support
#define SD_CMD_ARG_VDD              0xFF8000   //VDD Voltage Window  

/*ARG for cmd55*/
#define SD_CMD_ARG_DEFAULT_RCA     0x00

/*bus wide set*/
#define SD_DATA_BUS_BIT4    0x02
#define SD_DATA_BUS_BIT0    0x00
#define SD_DATA_BUS          SD_DATA_BUS_BIT4


/*switch func mode*/
typedef enum {
    SD_SfMode0,//Check function
    SD_SfMode1,//set function
}enSdSFModeDef;

#ifdef __cplusplus
}
#endif
#endif

