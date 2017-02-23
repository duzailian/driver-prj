#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define ON              1
#define OFF             0

typedef enum{
enRead,
enWrite
}enDataDirDef;//data transmission dir

typedef enum{
enCmd,
enData,
}enCmdDatDef;//data or cmd

typedef enum{
enOn,
enOff,
}enOnOffDef;//

#ifdef __cplusplus
}
#endif 
#endif

