#ifndef __INTERRUPT_H
#define __INTERRUPT_H
#ifdef __cplusplus
 extern "C" {
#endif 

typedef void (*pIrqFunc)(void);

extern void IrqRegester(IRQn_Type IRQn, pIrqFunc IrqFunc);
extern void InterruptInit(void);
extern void InterruptCfg(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);

#ifdef __cplusplus
}
#endif
#endif
