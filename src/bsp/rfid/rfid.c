#ifndef __RFID_C
#define __RFID_C

#ifdef __cplusplus
 extern "C" {
#endif 

#include "rc522/rc522.c"
#include "s50/s50.c"

/*
功能:
    RFID 模块初始化
参数:   void
返回值: void
输入:   void
输出:   void
备注:
*/
void RFIDInit(void)
{
    Rc522Init();
    S50DataInit();
    return;
}



#ifdef __cplusplus
}
#endif


#endif /* __RFID_C */
