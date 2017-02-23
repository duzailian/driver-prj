#ifndef __E2PROM_MGR_C
#define  __E2PROM_MGR_C

#ifdef __cplusplus
 extern "C" {
#endif 
/*
文件说明:
    1.E2PROM 管理相关程序
*/

/*
功能:
     S50相关数据结构初始化
参数:   void
返回值: void
输入:   void
输出:   void
备注:   无
注意:   无
 */
void E2PROM_Default(void)
{
    uint8 *pucTmp = NULL;
    uint16 usIndex = 0;
    enMemTypeDef enMemType = enMemSmall;

    pucTmp = MemMalloc(enMemType, &usIndex);
    if(pucTmp)
    {
        memset(pucTmp, MEM_SMALL_SZ, E2PROM_DEFAULT_VAL);
        E2PROM_Write(0, pucTmp, E2PROM_CAP);
        MemFree(enMemType, usIndex);
    }
    else
    {
        printf("E2PROM Default Get Memory Error!\r\n");
    }
    return;
}

#ifdef __cplusplus
}
#endif


#endif /* __E2PROM_MGR_C */


