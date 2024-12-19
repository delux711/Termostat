/* 
 * File:   1wire.h
 * Author: delux
 *
 * Created on Sobota, 2013, august 31, 13:17
 */

#ifndef WIRE_H
#define	WIRE_H

#include <stm32f10x.h>
#include <stdbool.h>

#define WIRE_EXTERNAL_SUPPLY
//#define WIRE_PARASITE_SUPPLY

#if defined(PIC)
#define WIRE_PORT     GPIOB      /* PORTC */
#define WIRE_TRIS     GPIOB->CRH /* TRISC */
#define WIRE_LAT      GPIOB->ODR /* LATC */
#define WIRE_DQin()   (((GPIOB->IDR & (1u<<13u))) != 0u) /* PORTCbits.RC2 */

#define WIRE_DQ       (GPIO_CRH_MODE13 | 0U)/*GPIO_CRH_CNF13_0)*/

#define WIRE_DQ_ON    GPIOB->ODR  |= GPIO_ODR_ODR13 /* WIRE_LAT |= WIRE_DQ */
#define WIRE_DQ_OFF   GPIOB->ODR &= ~GPIO_ODR_ODR13 /* WIRE_LAT &= ~WIRE_DQ */
#define WIRE_DQTRIS_OFF    {WIRE_TRIS |= GPIO_CRH_CNF13_1; WIRE_LAT = (1u<<13u); }  /* WIRE_TRIS |= WIRE_DQ */ /* VSTUP WITH PULL-/UP */
#define WIRE_DQTRIS_ON     {WIRE_LAT = ~(1u<<13u); WIRE_TRIS = (GPIO_CRH_CNF13_0 | GPIO_CRH_MODE13) /* WIRE_TRIS &= ~WIRE_DQ */ /* vystup with log0 */
#endif

#define WIRE_TRIS2test   // GPIOB_CRH// TRISBbits.TRISB0 = 0
#define WIRE_TEST 	//(1<<0)
#define WIRE_TEST_ON     //PORTB |= WIRE_TEST
#define WIRE_TEST_OFF    //PORTB &= ~WIRE_TEST

// vystup s log0 */
static inline void wirePinOutLog0(void) {
#if defined(PIC)
    WIRE_DQ_OFF;
    WIRE_DQTRIS_ON;
#else // stm32f103
  #ifdef WERE_REMAP1
    uint32_t temp = GPIOB->CRH;
    temp &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);
    temp |= GPIO_CRH_MODE13;  // Output mode, max speed 50 MHz
    temp |= GPIO_CRH_CNF13_0; // General purpose output Open-drain
    GPIOB->BSRR |= (GPIO_BSRR_BR0 << 13u); // GPIOx_BSRR_BRy - pin to log0 - pull-down
    GPIOB->CRH = temp; // output mode 50MHz as open-drain
  #else // !WERE_REMAP1
    uint32_t temp = GPIOB->CRH;
    temp &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);
    temp |= GPIO_CRH_MODE9;  // Output mode, max speed 50 MHz
    temp |= GPIO_CRH_CNF9_0; // General purpose output Open-drain
    GPIOB->BSRR |= (GPIO_BSRR_BR0 << 9u); // GPIOx_BSRR_BRy - pin to log0 - pull-down
    GPIOB->CRH = temp; // output mode 50MHz as open-drain
  #endif
#endif
}

// vstup s pull-up rezistorom
static inline void wirePinInPullUp(void) {
#if defined(PIC)    
    WIRE_DQTRIS_OFF;
#else // stm32f103
  #ifdef WERE_REMAP1
    uint32_t temp = GPIOB->CRH;
    temp &= ~(GPIO_CRH_CNF13 | GPIO_CRH_MODE13);    // Input mode (reset state)
    temp |= GPIO_CRH_CNF13_1; // Input with pull-up or pull-down
    GPIOB->CRH = temp; // Input with pull-down
    GPIOB->BSRR |= (GPIO_BSRR_BS0 << 13u); // input pin pull-up
  #else
    uint32_t temp = GPIOB->CRH;
    temp &= ~(GPIO_CRH_CNF9 | GPIO_CRH_MODE9);    // Input mode (reset state)
    temp |= GPIO_CRH_CNF9_1; // Input with pull-up or pull-down
    GPIOB->CRH = temp; // Input with pull-down
    GPIOB->BSRR |= (GPIO_BSRR_BS0 << 9u); // input pin pull-up
  #endif
#endif        
}

static inline bool wireGetPin(void) {
    return (0u != (GPIOB->IDR & GPIO_IDR_IDR9));
}


/* orig
#define WIRE_PORT     PORTC
#define WIRE_TRIS     TRISC
#define WIRE_LAT      LATC
#define WIRE_TRIS1    TRISCbits.TRISC2 = 1
#define WIRE_DQin     PORTCbits.RC2

#define WIRE_DQ       (1<<2)

#define WIRE_DQ_ON    WIRE_LAT |= WIRE_DQ
#define WIRE_DQ_OFF   WIRE_LAT &= ~WIRE_DQ
#define WIRE_DQTRIS_OFF    WIRE_TRIS |= WIRE_DQ
#define WIRE_DQTRIS_ON     WIRE_TRIS &= ~WIRE_DQ

#define WIRE_TRIS2test    TRISBbits.TRISB0 = 0
#define WIRE_TEST 	(1<<0)
#define WIRE_TEST_ON     GPIOB |= WIRE_TEST
#define WIRE_TEST_OFF    GPIOB &= ~WIRE_TEST


*/


#ifdef WIRE_PARASITE_SUPPLY
void wirePowerOn();
void wirePowerOff();
/*
#define WIRE_VPP_TRIS   TRISB
#define WIRE_VPP 	(1<<1)
#define WIRE_VPP_ON    WIRE_VPP_TRIS |= WIRE_VPP
#define WIRE_VPP_OFF   WIRE_VPP_TRIS &= ~WIRE_VPP
 */
#endif
void wireSendLog0();
void wireSendLog1();
unsigned char wireInitBus();
void wireSendByte(unsigned char znak);
unsigned char wireLoadByte();
unsigned char wireLoadBite();

extern void Delay_us(uint32_t us);

#endif	/* WIRE_H */
