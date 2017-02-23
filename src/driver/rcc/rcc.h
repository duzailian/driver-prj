#ifndef __RCC_H
#define __RCC_H
#ifdef __cplusplus
 extern "C" {
#endif 

#define RCC_APB2EN(Periph)                 (RCC_APB2Periph_##Periph)
#define RCC_APB1EN(Periph)                 (RCC_APB1Periph_##Periph)

#ifdef __cplusplus
}
#endif
#endif// __RCC_H
