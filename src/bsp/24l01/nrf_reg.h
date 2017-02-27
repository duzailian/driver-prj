#ifndef __NRF_REG_H
#define __NRF_REG_H

#ifdef __cplusplus
 extern "C" {
#endif 

typedef enum{
CONFIG,// 0x00
EN_AA,
EN_RXADDR,
SETUP_AW,
SETUP_RETR,
RF_CH,// 0x05
RF_SETUP,
STATUS,
OBSERVE_TX,
RPD,
RX_ADDR_P0,// 0x0a
RX_ADDR_P1,
RX_ADDR_P2,
RX_ADDR_P3,
RX_ADDR_P4,
RX_ADDR_P5,// 0x0f
TX_ADDR,
RX_PW_P0,
RX_PW_P1,
RX_PW_P2,
RX_PW_P3,
RX_PW_P4,// 0x15
RX_PW_P5,
FIFO_STATUS,
DYNPD = 0x1c,
FEATURE,
}enNrfRegDef;

#define REG_CONFIG
/*CONFIG Register*/
#define MASK_RX_DR          (1 << 6)
#define MASK_TX_DS          (1 << 5)
#define MASK_MAX_RT         (1 << 4)
#define EN_CRC              (1 << 3)
#define CRCO                (1 << 2)
#define PWR_UP              (1 << 1)
#define PRIM_RX             (1 << 0)

#define REG_EN_AA
/*EN_AA Register*/
#define ENAA_P5             (1 << 5)//Enable auto acknowledgement data pipe 5
#define ENAA_P4             (1 << 4)//Enable auto acknowledgement data pipe 4
#define ENAA_P3             (1 << 3)//Enable auto acknowledgement data pipe 3
#define ENAA_P2             (1 << 2)//Enable auto acknowledgement data pipe 2
#define ENAA_P1             (1 << 1)//Enable auto acknowledgement data pipe 1
#define ENAA_P0             (1 << 0)//Enable auto acknowledgement data pipe 0

#define REG_EN_RXADDR
/*EN_RXADDR Register*/
#define ERX_P5              (1 << 5)//Enable data pipe 5
#define ERX_P4              (1 << 4)//Enable data pipe 4
#define ERX_P3              (1 << 3)//Enable data pipe 3
#define ERX_P2              (1 << 2)//Enable data pipe 2
#define ERX_P1              (1 << 1)//Enable data pipe 1
#define ERX_P0              (1 << 0)//Enable data pipe 0

#define REG_SETUP_AW
/*SETUP_AW Register*/
#define AW                  3// RX/TX Address field width 

#define REG_SETUP_RETR
/*SETUP_RETR Register*/
//#define ARD                 (5 << 4)//Auto Retransmit Delay (n + 1) * 250us  (n <= 0x0f)
#define ARC                 (5)//Auto Retransmit Count(n <= 0x0f)

/*根据Auto Retransmit Delay获取ARD位段的值
(us必须是250us的整数倍,最大值为4000us,最小值为250us)
特别注意:
1.在2Mbps模式下, ACK payload大于15Bytes时,ARD必须大于500us
2.在1Mbps模式下, ACK payload大于5Bytes时,ARD必须大于500us
3.在250kbps模式下(即便没有ACK payload)时,ARD必须大于500us
*/
#define GetARD(us)          (((((us) - 250) / 250) & 0x0f) << 4)

#define REG_RF_CH
/*RF_CH Register*/
/* RF_CH 注意:
The channel occupies a bandwidth of less than 1MHz at 250kbps and 1Mbps 
and a bandwidth of less than 2MHz at 2Mbps. 
To ensure non-overlapping channels in 2Mbps mode, the channel spacing must be 2MHz or more. A
*/
#define RF_CH               (5)//Sets the frequency channel nRF24L01+ operates on (n <= 0x7f)

#define REG_RF_SETUP
/*RF_SETUP Register*/
#define CONT_WAVE            (1 << 7)//Enables continuous carrier transmit when high.
//#define RF_DR_LOW           (1 << 5)//Set RF Data Rate to 250kbps.if this bit is set,RF_DR_HIGH bit is don’t care
#define PLL_LOCK             (1 << 4)//Force PLL lock signal. Only used in test
//#define RF_DR_HIGH          (1 << 3)//Select between the high speed data rates
#define RF_DR_1M             (0 << 3)
#define RF_DR_2M             (1 << 3)
#define RF_DR_250K           (4 << 3)

#define RF_PWR_18dB          (0 << 1)//Set RF output power in TX mode -18dBm
#define RF_PWR_12dB          (1 << 1)//Set RF output power in TX mode -12dBm
#define RF_PWR_6dB           (2 << 1)//Set RF output power in TX mode -6dBm
#define RF_PWR_0dB           (3 << 1)//Set RF output power in TX mode  0dBm


#define REG_STATUS
/*STATUS Register*/
#define RX_DR               (1 << 6)//Data Ready RX FIFO interrupt(Write 1 to clear bit.)
#define TX_DS               (1 << 5)//Data Sent TX FIFO interrupt(Write 1 to clear bit.)
#define MAX_RT              (1 << 4)//Maximum number of TX retransmits interrupt(Write 1 to clear bit.)
/*RX_P_NO:
Data pipe number for the payload available for reading from RX_FIFO
000-101: Data Pipe Numbe
110: Not Used
111: RX FIFO Empty*/
#define RX_P_NO             (0x07 << 1)
#define TX_FULL             (1)//Enable data pipe 2

#define REG_OBSERVE_TX
/*OBSERVE_TX Register*/
#define PLOS_CNT            (0x0f << 4)//Count lost packets. 
#define ARC_CNT             (0x0f << 0)//Count retransmitted packets.

#define REG_RPD
/*RPD Register*/
#define RPD                  (1 << 0)//Received Power Detector

/*
RX_ADDR_P0--1:
Receive address data pipe 0. 5 Bytes maximum 
length. (LSByte is written first. Write the number of 
bytes defined by SETUP_AW)*/
#define REG_RX_ADDR_P0
/*RX_ADDR_P0 Register*/
#define REG_RX_ADDR_P1
/*RX_ADDR_P1 Register*/

/*
Receive address data pipe 2--5. Only LSB. MSBytes 
are equal to RX_ADDR_P1[39:8]
*/
#define REG_RX_ADDR_P2
/*RX_ADDR_P2 Register*/
#define REG_RX_ADDR_P3
/*RX_ADDR_P3 Register*/
#define REG_RX_ADDR_P4
/*RX_ADDR_P4 Register*/
#define REG_RX_ADDR_P5
/*RX_ADDR_P5 Register*/

#define REG_TX_ADDR//Transmit address.
/*TX_ADDR Register*/

/*REG_RX_PW_Pn:
Number of bytes in RX payload in data pipe 0 (1 to 
32 bytes). 
0 Pipe not used
1 = 1 byte
…
32 = 32 bytes
*/
#define REG_RX_PW_PnMASK        0x3f
#define REG_RX_PW_P0
/*RX_PW_P0 Register*/
#define REG_RX_PW_P1
/*RX_PW_P1 Register*/
#define REG_RX_PW_P2
/*RX_PW_P2 Register*/
#define REG_RX_PW_P3
/*RX_PW_P3 Register*/
#define REG_RX_PW_P4
/*RX_PW_P4 Register*/
#define REG_RX_PW_P5
/*RX_PW_P5 Register*/


#define REG_FIFO_STATUS
/*FIFO_STATUS Register*/
#define TX_REUSE             (1 << 6)
#define TX_FULL              (1 << 5)
#define TX_EMPTY             (1 << 4)
#define RX_FULL              (1 << 1)
#define RX_EMPTY             (1 << 0)


#define REG_DYNPD
/*DYNPD Register*/
#define DPL_P5              (1 << 5)//Enable dynamic payload length data pipe 5. (Requires EN_DPL and ENAA_P5)
#define DPL_P4              (1 << 4)//Enable dynamic payload length data pipe 4. (Requires EN_DPL and ENAA_P4)
#define DPL_P3              (1 << 3)//Enable dynamic payload length data pipe 3. (Requires EN_DPL and ENAA_P3)
#define DPL_P2              (1 << 2)//Enable dynamic payload length data pipe 2. (Requires EN_DPL and ENAA_P2)
#define DPL_P1              (1 << 1)//Enable dynamic payload length data pipe 1. (Requires EN_DPL and ENAA_P1)
#define DPL_P0              (1 << 0)//Enable dynamic payload length data pipe 0. (Requires EN_DPL and ENAA_P0)

#define REG_FEATURE
/*Feature Register*/
#define EN_DPL              (1 << 2)//Enables Dynamic Payload Length
#define EN_ACK_PAY          (1 << 1)//Enables Payload with ACK
#define EN_DYN_ACK          (1 << 0)//Enables the W_TX_PAYLOAD_NOACK command

#ifdef __cplusplus
}
#endif
#endif//__NRF_REG_H
