#ifndef __S50_H
#define __S50_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define S50DBG              1

#define S50_KEYA_SZ              6

extern void S50Test(void);
extern void S50SendHLTA(void);
extern bool S50SendWUPA(void);
extern bool S50CardDetect(void);
extern bool S50MFAuthent(uint8 ucBlockAddr);//хож╓
extern void S50DataInit(void);
extern bool S50MFBlockWrite(uint8 ucBlockAddr, uint32 ulVal);
extern bool S50MFBlockRead(uint8 ucBlockAddr, uint32 *pulVal);

#ifdef __cplusplus
}
#endif


#endif /* __S50_C */



