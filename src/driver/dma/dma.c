#ifndef __DMA_C
#define __DMA_C

/*rw enRead ¶Á¿¨, enWrite Ð´¿¨*/
void DmaSdioCfg(enDataDirDef rw,uint8 *pucData, uint16 usDataSz)
{
    bool bIsRead = (rw == enRead);
    
    DMA2_Channel4->CPAR  = (uint32_t)&(SDIO->FIFO);
    DMA2_Channel4->CMAR  = (uint32_t)pucData;
    DMA2_Channel4->CNDTR = usDataSz / 4;
    DMA2_Channel4->CCR   = DMA_Priority_VeryHigh | DMA_PeripheralDataSize_Word | DMA_MemoryDataSize_Word | DMA_MemoryInc_Enable
                            | DMA_PeripheralInc_Disable | DMA_Mode_Normal | (bIsRead ? DMA_DIR_PeripheralSRC : DMA_DIR_PeripheralDST);
                            //| DMA_IT_TC | DMA_IT_TE;

    EnableSdioDma();
    return;
}

/*0: ³É¹¦, 1:Ê§°Ü*/
uint16 DmaSdioWaitCmpt(void)
{//IS_DMA_GET_IT
    uint16 err = 0;
    
    while(0 == (DMA2->ISR & DMA2_FLAG_TC4))
    {
        if (DMA2->ISR & DMA2_FLAG_TE4)
        {//err
            err = 1;
            goto ErrReturn ;
        }
    }
ErrReturn:
    DMA2->IFCR = DMA2_FLAG_TE4 | DMA2_FLAG_TC4 | DMA2_FLAG_HT4;
    DisableSdioDma();
    return err;
}

#endif
