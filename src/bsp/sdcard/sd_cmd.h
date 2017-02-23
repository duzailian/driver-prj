#ifndef __SD_CMD_H
#define __SD_CMD_H

#ifdef __cplusplus
 extern "C" {
#endif

#define SD_CMD_MASK                      (0x3f)
/*Basic Commands (class 0) */
#define SD_CMD_GO_IDLE_STATE            (0)
#define SD_CMD_ALL_SEND_CID              (2)
#define SD_CMD_SEND_RCA                  (3)
#define SD_CMD_SET_DSR                   (4)
#define SD_CMD_SWITCH_FUNC               (6)
#define SD_CMD_SELECT_CARD               (7)
#define SD_CMD_SEND_IF_COND              (8)
#define SD_CMD_SEND_CSD                  (9)
#define SD_CMD_SEND_CID                  (10)
#define SD_CMD_STOP_TRANSMISSION        (12)
#define SD_CMD_SEND_STATUS              (13)
#define SD_CMD_GO_INACTIVE_STATE        (15)

/*Block-Oriented Read Commands (class 2) */
#define SD_CMD_SET_BLOCKLEN             (16)
#define SD_CMD_READ_SINGLE_BLOCK        (17)
#define SD_CMD_READ_MULTIPLE_BLOCK      (18)

/*Block-Oriented Write Commands (class 4) */
#define SD_CMD_WRITE_BLOCK               (24)
#define SD_CMD_WRITE_MULTIPLE_BLOCK     (25)
#define SD_CMD_PROGRAM_CSD               (27)

/*Erase Commands (class 5) */
#define SD_CMD_ERASE_WR_BLK_START       (32)
#define SD_CMD_ERASE_WR_BLK_END         (33)
#define SD_CMD_SEND_ERASE                (38)

/*Block Oriented Write Protection Commands (class 6) */
#define SD_CMD_SET_WRITE_PROT           (28)
#define SD_CMD_CLR_WRITE_PROT           (29)
#define SD_CMD_SEND_WRITE_PROT          (30)

/*Lock Card (class 7) */
#define SD_CMD_LOCK_UNLOCK              (42)

/*Application-specific Commands (class 8) */
#define SD_CMD_APP_CMD                  (55)
#define SD_CMD_GEN_CMD                  (56)

#define SD_CMD_SET_BUS_WIDTH           (6)
#define SD_CMD_SD_STATUS               (13)
#define SD_CMD_SEND_NUM_WR_BLOCKS     (22)
#define SD_CMD_SET_WR_BLK_ERASE_COUNT (23)
#define SD_CMD_SD_SEND_OP_COND        (41)
#define SD_CMD_SET_CLR_CARD_DETECT    (42)
#define SD_CMD_SEND_SCR                (51)

#ifdef __cplusplus
}
#endif

#endif /*__STM32F10x_ADC_H */

