
/* Includes ------------------------------------------------------------------*/
#include "include.h"
char *pcTmp =   "经过几个月来业余时间夜以继日的不懈努力，\r\n"
                "小杜终于仅靠stm32的reference manual\r\n"
                "和Sd Card的Specification两份全英文文档，\r\n"
                "完成了全自主的sdio驱动sd卡程序设计,\r\n"
                "并成功完成基于该驱动的FATFS文件系统移植与调试\r\n"
                "您现在看到的这段文字，正是我在开发板上用该FATFS写入到SD卡的\r\n"
                "仅仅借用windows的屏幕驱动显示而已。\r\n"
                "这心旷神怡，是无论把多少位女神看成老太婆都看不来的。\r\n"
                "程序设计是门需要精雕细琢的艺术，程序猿们需要\r\n"
                "在让机器的资源尽可能高效利用的前提下，不断\r\n"
                "优化程序框架、规范代码编写风格以便他人阅读及Debug.\r\n"
                "物联网时代已经来了，骚年，睡什么觉? "
                "养家糊口貌似只能靠钱，而不是连糖纸都换不来的扯犊子的梦想。---杜在连2017-1-12";
//static  __align(4) uint8 aucSdWriteBuffer[] = "asdea8csa2asxa";
//static  __align(4) uint8 aucSdReadBuffer[512 * 3] = {0};

int main(void)
{
    DriverInit();
    BspInit();
    MiddleInit();

#if DELAY_DBG
    DelayTest();
#endif

#if SD_DBG//SD CARD
    SdTest();
#endif

#if FLASH_DBG//W25Q64
    W25Q64Test();
#endif

#if OLED_DBG//OLED
    OLED_Test();
#endif

#if FATFS_DBG//FATFS
    FatfsTest(pcTmp);
#endif

#if E2PROM_DBG
    E2PROM_Test();
#endif

#if BEEP_DBG
    BeepTest();
#endif

#if RC522_DBG
    Rc522SelfTest();
#endif

#if S50DBG//RC522
    S50Test();
#endif
    while(1);
    return 0;
}
