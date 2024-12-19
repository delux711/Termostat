#include <stm32f10x.h>
#include "system_stm32f10x.h"
#include "temp_DS18B20.h"
#include "delay.h"
#include "i2c_test.h"

int main(void) {
    uint8_t count2 = 0u;
    uint16_t count16;
    uint32_t reg;
    ROM tempRomValue;
    static volatile TEPLOMER tepl = { 0u };
    
    #ifdef VECT_TAB_SRAM
      SCB->VTOR = SRAM_BASE;// | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM. */
    #else
      SCB->VTOR = FLASH_BASE;// | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH. */
    #endif 
    SystemInit();
		SystemCoreClockUpdate();
    SysTick_Init(); // Inicializace SysTick
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;

    RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
    reg = GPIOC->CRH;
    reg &= ~((uint32_t)GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    reg |= GPIO_CRH_MODE13; // 11: Output mode, max speed 50 MHz
    GPIOC->CRH = reg;
		
		GPIOC->BSRR = (GPIOC->ODR & GPIO_ODR_ODR13) ? GPIO_BSRR_BR13 : GPIO_BSRR_BS13;
		Delay_us(1000u);
		GPIOC->BSRR = (GPIOC->ODR & GPIO_ODR_ODR13) ? GPIO_BSRR_BR13 : GPIO_BSRR_BS13;
		Delay_us(500u);
		GPIOC->BSRR = (GPIOC->ODR & GPIO_ODR_ODR13) ? GPIO_BSRR_BR13 : GPIO_BSRR_BS13;
		Delay_us(2000u);
		GPIOC->BSRR = (GPIOC->ODR & GPIO_ODR_ODR13) ? GPIO_BSRR_BR13 : GPIO_BSRR_BS13;
		Delay_us(100000u);
		GPIOC->BSRR = (GPIOC->ODR & GPIO_ODR_ODR13) ? GPIO_BSRR_BR13 : GPIO_BSRR_BS13;

    count2 = 0u;
    
    tempInic();
		tempRomSearch(0u);
    // tempRomValue = temp1();
    
    tempSetDefaultTemp(temp1());
    tempStartConvert();
    tepl = tempLoadData();
	
    for(;;) {
        // bit banding pre PC13 ODR registra je adresa 0x422201B4 = 0x42000000 + (0x1100c*32)+(0xD*4)
        // adresa bitu BS13 pre PORTC je: 0x42220234 = 0x42000000 + (0x11010*32)+(13*4)
        // adresa bitu BR13 pre PORTC je: 0x42220274 = 0x42000000 + (0x11010*32)+(29*4)
        //GPIOC->BSRR = (GPIOC->ODR & GPIO_ODR_ODR13) ? GPIO_BSRR_BR13 : GPIO_BSRR_BS13;
        if(count16 == 0u) {
            if(25u < count2) {
                count2 = 0u;
                GPIOC->BSRR = (GPIOC->ODR & GPIO_ODR_ODR13) ? GPIO_BSRR_BR13 : GPIO_BSRR_BS13;
            }
            count2++;
        }
        count16++;
		i2c_handleTask();
    }
    return 0;
}
