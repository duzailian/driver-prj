#ifndef __E2PROM_C
#define __E2PROM_C

/*
文件说明:
    1.AT24C01A/AT24C02/AT24C04/AT24C08/AT24C16 驱动程序
*/

#define E2PROM_I2C_CH         I2cCh1
#if (E2PROM_TYPE == E2PROM_24C02)
#define E2PROM_I2C_ADDR       (0x50)
#endif

#if (E2PROM_TYPE > E2PROM_24C02)
#define E2PROM_PAGE_SZ       16
#else
#define E2PROM_PAGE_SZ       8
#endif

void E2PROM_Init(void)
{
    I2cInit(E2PROM_I2C_CH);
    return;
}

uint8 E2PROM_ReadByte(uint8 ucAddr)
{
    uint8 ucTmp = 0;

    ucTmp = ucAddr;
    I2cWrite(E2PROM_I2C_CH, E2PROM_I2C_ADDR, &ucTmp, sizeof(ucTmp));
    I2cRead(E2PROM_I2C_CH, E2PROM_I2C_ADDR, &ucTmp, sizeof(ucTmp));
    
    return ucTmp;
}

void E2PROM_WriteByte(uint8 ucAddr, uint8 ucData)
{
    uint8 aucTmp[2] = {0};
    uint8 ucRead = 0;

    ucRead = E2PROM_ReadByte(ucAddr);
    if(ucData != ucRead)
    {//读取跟写入不一致才执行写入操作
        aucTmp[0] = ucAddr;
        aucTmp[1] = ucData;
        I2cWrite(E2PROM_I2C_CH, E2PROM_I2C_ADDR, &aucTmp[0], sizeof(aucTmp));
    }
    return;
}

/*
功能:
    1.向E2PROM写入1PAGE的数据
参数:   
    1.usAddr    数据在E2PROM中的地址
    2.ucDataSz  数据长度
返回值: void
输入:   void
输出:   void
备注:   
    1.The AT24C01/AT24C02 EEPROM is capable of an 8-byte page write, and the AT24C04/AT24C08A
      and AT24C16A devices are capable of 16-byte page writes.
*/
static void E2PROM_WritePage(uint8 ucAddr, uint8 *pucData, uint8 ucDataSz)
{
    uint8 aucTmp[E2PROM_PAGE_SZ + 1] = {0};
    uint8 i = 0;
    
    aucTmp[i++] = ucAddr;
    memcpy(&aucTmp[i], pucData, ucDataSz);
    I2cWrite(E2PROM_I2C_CH, E2PROM_I2C_ADDR, &aucTmp[0], ucDataSz + 1);
    DelayMs(5);
    return;
}

void E2PROM_Read(uint8 ucAddr, uint8 *pucRead, uint16 ucReadSz)
{
    I2cStart(E2PROM_I2C_CH, E2PROM_I2C_ADDR, enWrite);
    I2cWriteByte(E2PROM_I2C_CH, ucAddr);
    I2cRead(E2PROM_I2C_CH, E2PROM_I2C_ADDR, pucRead, ucReadSz);
    //DelayMs(10);
    return;
}
/*
功能:
    1.写入E2PROM中usAddr地址上的长度为ucDataSz的数据
参数:   
    1.usAddr    数据在E2PROM中的地址
    2.ucDataSz  数据长度
    3.pucWrite  指向缓存的指针
返回值: void
输入:   void
输出:   void
备注:   
    1.usAddr通过GetE2promAddr(Obj)获得，其中Obj为stE2promIndexDef中的对应成员
*/
void E2PROM_Write(uint8 ucAddr, uint8 *pucWrite, uint16 usWriteSz)
{
    uint8 i = 0;
    uint8 ucPageNum = 0;//用于存储写入Page的个数

    ucPageNum = usWriteSz / E2PROM_PAGE_SZ;
    for(i= 0 ; i < ucPageNum; i++)
    {
        E2PROM_WritePage(ucAddr, pucWrite, E2PROM_PAGE_SZ);
        ucAddr += E2PROM_PAGE_SZ;
        usWriteSz -= E2PROM_PAGE_SZ;
        pucWrite += E2PROM_PAGE_SZ;
    }
    E2PROM_WritePage(ucAddr, pucWrite, usWriteSz);
    return;
}
#if E2PROM_DBG
#define E2TEST_LEN      E2PROM_CAP//测试数据长度
#define E2TEST_TIMES    100//测试次数

uint8 aucE2Write[E2TEST_LEN] = {0};
/*
功能:
    1.填充E2PROM驱动程序测试数据到缓存aucE2Test[100]
参数:   void
返回值: void
输入:   void
输出:   void
备注:   
*/
void E2PROM_FullData(void)
{
    uint16 usLen = 0;//已获取到的数据长度
    bool   bRes = FALSE;
    uint8  ucCnt = sizeof(aucE2Write) / RANDOM_SZ;
    uint8  i = 0;
    uint8  aucTmp[RANDOM_SZ] = {0};
    
    for(i = 0; i < ucCnt; i++)
    {
        bRes = Rc522GetRanData(&aucE2Write[usLen]);
        if (FALSE == bRes)
        {
            printf("E2Test FullData Err!\r\n");
            break;
        }
        usLen += RANDOM_SZ;
    }
    bRes = Rc522GetRanData(&aucTmp[0]);
    memcpy(&aucE2Write[usLen], &aucTmp[0], sizeof(aucE2Write) % RANDOM_SZ);
    return;
}

/*
功能:
    1.E2PROM驱动程序测试
参数:   void
返回值: void
输入:   void
输出:   void
备注:   
*/
void E2PROM_Test(void)
{
    static uint8 aucE2Read[E2TEST_LEN] = {0};
    uint8  i = 0;

    for (i = 0; i < E2TEST_TIMES; i++)
    {
        E2PROM_FullData();
        E2PROM_Write(0, aucE2Write, sizeof(aucE2Write));
        E2PROM_Read(0, aucE2Read, sizeof(aucE2Read));
        if(memcmp(aucE2Write, aucE2Read, E2TEST_LEN))
        {
            printf("E2 Test(%d) Err!\r\n", i);
            goto ErrReturn;
        }
    }
    printf("E2 Test Success!\r\n");
    UartPrintBuffer(aucE2Write, sizeof(aucE2Write));
ErrReturn:
    return;
}
#endif

#endif
