#ifndef __NRF_CMD_H
#define __NRF_CMD_H

#ifdef __cplusplus
 extern "C" {
#endif 

#define R_REGISTER              0x00
#define W_REGISTER              0x20
#define R_RX_PAYLOAD            0x61
#define W_TX_PAYLOAD            0xA0
#define FLUSH_TX                0xE1
#define FLUSH_RX                0xE2
#define REUSE_TX_PL             0xE3
#define R_RX_PL_WID             0x60
#define W_ACK_PAYLOAD           0xA8
#define W_TX_PAYLOAD_NO_ACK    0xB0
#define NOP                      0xFF

#ifdef __cplusplus
}
#endif
#endif//__NRF_CMD_H

