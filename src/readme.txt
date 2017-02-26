待完成:
1.调试OLED驱动
2.查找SDIO DMA "认内存"原因
3.完成并调试MFRC522读写S50卡程序
4.添加内存管理机制并调试
log:
17-1-16 13:34
	1.添加rfid文件夹到工程(待调试)
	2.修改spi文件夹下的内容(待调试)
17-1-17 16:15
	1.将SPI CS下放到SPI驱动中
17-1-18 17:07
	1.基本完成W25Q64操作函数(待调试)
	2.添加common.h和err.h用于统一数据操作方式及错误代码
17-1-18 20:00
	1.通过将写缓存固定到指定内存，暂时解决不能正常格式化问题
17-1-19 22:00
	1.调试完成w25q64驱动v0.1
17-1-20 09:30
	1.合入OLED及I2C驱动程序
	2.添加 ON/OFF、HIGH/LOW、宏定义及GpioOut、GpioIn定义
17-1-22 17:11
	1.添加内存管理函数(待确认)
	2.将RC522SpiRead()改为只重复发送同一字节
17-1-23 17:27
	1.RC522SpiRead()函数写入长度为0时表示只重复发送同一字节
17-1-24 13:40
	1.添加interrupt目录及.c/.h文件到driver目录下
17-1-25 1:40
	1.将OLED改为SPI模式(待调试)
17-1-25 16:33
	1.添加MiddleInit.c/driver_init.c/bsp_init.c
17-2-10 0:06
	已完成:
	1.memory manage调试完成
	2.rc522配置(暂时不用)
	3.rc522 rst引脚配置及Rc522Rst()函数实现
	4.用Rc522GetColl()函数获取碰撞bit位置
	5.调试完成fifo写操作
	6.RC522SpiRead()读函数调试完成
	待完成:
	1.调试通过rc522 self test
	2.Rc522SendData(),Rc522SendDataCrc();
	3.S50卡驱动编写与调试
17-2-13 11:27
	待完成:
	1.处理延时函数里rtc/systick溢出引发的bug
17-2-16
	1.将Rc522SendCmd()函数改为根据cmd判断是否等待需要等待cmd完成标志
	2.添加Rc522IrqClr()宏定义，用于清空两个Irq寄存器的所有内容
	3.删掉所有原_init.c/.h结尾的文件，将初始化函数定义/声明添加到对应的.c文件中如:driver init添加到driver.c中
	4.将common.h移到src/main目录下
	5.完成S50CardDetect()函数(待调试)
	6.添加E2PROM 管理相关程序
17-2-17
	1.添加Rc522MFAuthent()函数(待调试)
	2.添加s50reg.h文件
	3.完成S50MFAuthent()函数(待调试)
	4.完成S50卡BLOCK read/write功能(待调试)
17-2-19 
	1.E2PROM测试通过；
	2.修复delayms的bug
	3.添加beep函数
	4。完成E2 Default功能
17-2-20
	1.在FIFO读取函数中添加形参表示请求读取的数据长度，如果FIFO中的实际长度与所需长度不一致，则
	  返回FALSE;
17-2-24
	1.Rc522WaitFlg()等到指定Flg,则清空对应寄存器的对应bit
	2.
