#ifndef __S50_REG_H
#define __S50_REG_H

#ifdef __cplusplus
 extern "C" {
#endif 
/*
文件说明:
    1.定义了S50卡的Block、Access conditions等信息
备注:
    1. The 1024 X 8 bit EEPROM memory is organized in 16 sectors of 4 blocks
    2. One block contains 16 bytes
    3. Block0 Sector0 is Manufacturer Data
    4. 
*/

#define S50VAL_DATA_SZ      4//1个Block中存储的有效的数据长度
#define S50BLOCK_DATA_SZ    16//1个Block中存储的总数据长度

/*
stS50DataBlockDef 
说明: 
    1.给出了DataBlock(Block0 - Block2)的数据组成
    2.小端存储
备注:
    1.S50卡每个Block存储16Bytes数据,实际有效的数据为4Byte(数据) + 1Byte(可用于存储备份Block地址)
      其余均为防错用的重复值
      具体格式如下:
        value(4Byte) + /value(4Byte) + value(4Byte) + addr + /addr + addr + /addr
        其中,addr可以用于记录备份Block地址,/value表示value按位取反后的值
*/
typedef struct{
unWordDef unVal0;//non-inverted
unWordDef _unVal;//inverted Data
unWordDef unVal1;//non-inverted

uint8 ucAddr0;//non-inverted
uint8 _ucAddr0;//inverted Data
uint8 ucAddr1;//non-inverted
uint8 _ucAddr1;//inverted Data
}stS50DataBlockDef;

#ifdef __cplusplus
}
#endif


#endif /* __S50_REG_H */

