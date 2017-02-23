#ifndef __SDSTATE_H
#define __SDSTATE_H

#ifdef __cplusplus
 extern "C" {
#endif 

/*card state*/
#define CS_COM_CRC_ERROR            (1 << 23)
#define CS_ILLEGAL_COMMAND          (1 << 22)
#define CS_ERROR                     (1 << 19)

#define SD_STATE_APP_CMD            ((uint32)1 << 5)

#define SD_READY_FOR_DATA           ((uint32)1 << 8)

#define SD_STA_SZ                   (512 / 8)

#pragma pack(1)

typedef union{   
    struct {
        uint16  DAT_BUS_WIDTH : 2;
        uint16  SECURED_MODE  : 1;
        uint16  reserved2     : 13;
        uint16  SD_CARD_TYPE;
        
        uint32  SIZE_OF_PROTECTED_AREA;
        
        uint16   SPEED_CLASS       : 8;
        uint16   PERFORMANCE_MOVE : 8;


        uint32  AU_SIZE       :  4;
        uint32  reserved1     :  4;
        uint32  ERASE_SIZE    :  16;
        uint32  ERASE_TIMEOUT :  6;
        uint32  ERASE_OFFSET  :  2;

        uint8   reserved0[50];

    }SdSta;
    uint8 DATA[SD_STA_SZ];
}unSdStaDef;

typedef union{   
    struct {
        uint16  MDT : 12;
        uint16  res1 : 4;
        uint8   PSN[4];
        uint8   PRV;
        uint8   PNM[5];
        char    OID[2];
        uint8   MID;
    }CID;
    uint8 DATA[CID_SZ];
}unSdCardStaDef;

#pragma pack()


#ifdef __cplusplus
}
#endif
#endif /* __SDIO_H */


