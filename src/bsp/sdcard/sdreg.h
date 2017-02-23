#ifndef __SDREG_H
#define __SDREG_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define     CID_SZ               (120 / 8)
#define     CSD_SZ               (128 / 8)

#define     SD_RES_SHORT_SZ     48
#define     SD_RES_LONG_SZ      128

#pragma pack(1)
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
}unSdCidDef;

typedef union {
    struct{   
        uint32 res0 : 8;
        uint32 res1 : 2;
        uint32 FILE_FORMAT : 2;
        uint32 TMP_WRITE_PROTECT : 1;
        uint32 PERM_WRITE_PROTECT : 1;//14
        uint32 COPY :1;
        uint32 FILE_FORMAT_GRP : 1;
        uint32 res2 : 5;// 21
        uint32 WRITE_BL_PARTIAL : 1;
        uint32 WRITE_BL_LEN : 4;
        uint32 R2W_FACTOR : 3;
        uint32 res3 : 2;
        uint32 WP_GRP_ENABLE : 1;//32
        
        uint16 WP_GRP_SIZE : 7;
        uint16 SECTOR_SIZE : 7;
        uint16 ERASE_BLK_EN : 1;
        uint16 res4  : 1;//16
        
        uint32 C_SIZE : 22;
        uint32 res5 : 6;
        uint32 DSR_IMP : 1;
        uint32 READ_BLK_MISALIGN : 1;
        uint32 WRITE_BLK_MISALIGN : 1;
        uint32 READ_BL_PARTIAL : 1;//32
        
        uint32 READ_BL_LEN : 4;
        uint32 CCC : 12;
        uint32 TRAN_SPEED : 8;
        uint32 NSAC : 8;
        uint32 TAAC : 8;
        uint32 res6 : 6;
        uint32 CSD_STRUCTURE : 2;
    }CSD;
    uint8 DATA[CSD_SZ];
}SdCsdDef;

#pragma pack()
static unSdCidDef unCidBuf;
static SdCsdDef   unCsdBuf;

#if 0
#define MDT         (unCidBuf.CID.MDT)
#define PSN         (unCidBuf.CID.PSN)
#define PRV         (unCidBuf.CID.PRV)
#define PNM         (unCidBuf.CID.PNM)
#define OID         (unCidBuf.CID.OID)
#define MID         (unCidBuf.CID.MID)
#endif
typedef enum {
    SD_CID,//Host  Capacity Support
    SD_CSD,//Standard Capacity 
}enSdR2TypeDef;

#ifdef __cplusplus
}
#endif

#endif
