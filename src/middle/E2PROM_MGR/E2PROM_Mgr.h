#ifndef __E2PROM_MGR_H
#define  __E2PROM_MGR_H

#ifdef __cplusplus
 extern "C" {
#endif 
/*
说明:
    1.E2PROM 管理相关程序
*/
/****************************************************************************************/
#define E2PROM_DEFAULT_VAL       0xff//执行E2PROM 初始化时写入E2PROM的值

/****E2PROM存储索引定义***/
#pragma pack(1)
typedef struct{
uint8 S50KeyA[S50_KEYA_SZ];
}stE2promIndexDef;
#pragma pack()

/********E2PROM存储索引总空间********/
#define E2PROM_INDEX_SZ		(S50_KEYA_SZ)

#if (E2PROM_CAP < E2PROM_INDEX_SZ)
#error "E2PROM Capacity Err!"
#endif

/*获取Obj在E2PROM中的存储地址(Obj为stE2promIndexDef中的成员)*/
#define GetE2promAddr(Obj)         offsetof(stE2promIndexDef, Obj)

extern void E2PROM_Default(void);

#ifdef __cplusplus
}
#endif


#endif /* __E2PROM_MGR_H */


