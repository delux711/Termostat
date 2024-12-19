#include "delay.h"
#include <stm32f10x.h>

static volatile uint32_t usTicks = 0; // Glob�ln� promenn� pro uchov�n� poctu mikrosekund od spu�ten�

void SysTick_Handler(void) {
/*	static volatile uint8_t tick = 0u;
	tick++;
	if(tick == 100u) {
		tick = 0u;
		usTicks++; // Inkrementuj pocet mikrosekund
	}*/
	usTicks+=10u;
}

void SysTick_Init(void) {	
    SysTick->LOAD = 30u - 1u;// (SystemCoreClock / 8U) / (30000U); // Nastaven� c�tace pro generov�n� preru�en� ka�dou mikrosekundu
    SysTick->VAL = 0; // Nastaven� hodnoty c�tace na nulu
    SysTick->CTRL = /* SysTick_CTRL_CLKSOURCE_Msk |*/ SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; // Nastaven� SysTick jako zdroje hodin, povolen� preru�en� a spu�ten� casovace
}

void Delay_us(uint32_t us) {
    uint32_t start = usTicks; // Ulo�en� aktu�ln� hodnoty mikrosekund

    while ((usTicks - start) < us); // Cek�n�, dokud neplyne po�adovan� cas
}
