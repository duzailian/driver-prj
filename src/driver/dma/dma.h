#ifndef __DMA_H
#define __DMA_H

#ifdef __cplusplus
 extern "C" {
#endif

#define DisableSdioDma()    do{DMA2_Channel4->CCR &= ~DMA_EN;}while(0)
#define EnableSdioDma()     do{DMA2_Channel4->CCR |= DMA_EN;}while(0)

/*rw enRead ¶Á¿¨, enWrite Ð´¿¨*/
void DmaSdioCfg(enDataDirDef rw,uint8 *pucData, uint16 usDataSz);

/*0: ³É¹¦, 1:Ê§°Ü*/
uint16 DmaSdioWaitCmpt(void);

#ifdef __cplusplus
}
#endif

#endif /* __DMA_H */

