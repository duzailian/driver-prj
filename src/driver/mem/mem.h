#ifndef __MEM_H
#define __MEM_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define MEM_BIG_SZ               1024
#define MEM_MIDDLE_SZ            512
#define MEM_SMALL_SZ             256
#define MEM_SMALL64_SZ           64

typedef enum{
enMemBig,//1024
enMemMiddle,//512
enMemSmall,//256
enMemSmall64,//64
}enMemTypeDef;

void MemInit(void);
void *MemMalloc(enMemTypeDef enType, uint16 *usIndex);
void MemFree(enMemTypeDef enType, uint16 usIndex);

#ifdef __cplusplus
}
#endif
#endif


