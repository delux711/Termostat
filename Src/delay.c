#include "delay.h"
#include <stm32f10x.h>

static volatile uint32_t usTicks = 0; // Globální promenná pro uchování poctu mikrosekund od spuštení

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
    SysTick->LOAD = 30u - 1u;// (SystemCoreClock / 8U) / (30000U); // Nastavení cítace pro generování prerušení každou mikrosekundu
    SysTick->VAL = 0; // Nastavení hodnoty cítace na nulu
    SysTick->CTRL = /* SysTick_CTRL_CLKSOURCE_Msk |*/ SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; // Nastavení SysTick jako zdroje hodin, povolení prerušení a spuštení casovace
}

void Delay_us(uint32_t us) {
    uint32_t start = usTicks; // Uložení aktuální hodnoty mikrosekund

    while ((usTicks - start) < us); // Cekání, dokud neplyne požadovaný cas
}
