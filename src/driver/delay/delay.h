#ifndef __DELAY_H
#define __DELAY_H

#ifdef __cplusplus
 extern "C" {
#endif

#define DELAY_DBG       0

void DelayMs(uint16 usMs);
void DelayInit(void);
void DelayUs(uint16 usUs);
#if DELAY_DBG
void DelayTest(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MISC_H */

