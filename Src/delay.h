#ifndef DELAY_H
#define	DELAY_H
#include <stm32f10x.h>

extern void SysTick_Init(void);
extern void Delay_us(uint32_t us);

#endif // DELAY_H