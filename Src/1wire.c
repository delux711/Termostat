#include "1wire.h"

/**
 * Inicializuje zaciatok prenosu na zbernici
 * @return 1 ak chyba, inak 0
 */
unsigned char wireInitBus() {
    unsigned char i = 255;
    WIRE_TRIS2test;
    wirePinOutLog0();
    Delay_us(500);
    wirePinInPullUp();
    Delay_us(80);
    if (wireGetPin()) return 1;
    while (!wireGetPin()) {
        Delay_us(2);
        if (!i--) return 1;
    }
    Delay_us(60);
    return 0;
}

/**
 * Odosle logicku nulu
 */
void wireSendLog0() {
    wirePinOutLog0();
    Delay_us(60);
    wirePinInPullUp();
    Delay_us(2);
}

/**
 * * Odosle logicku jednotku
 */
void wireSendLog1() {
    wirePinOutLog0();
    Delay_us(2);
    wirePinInPullUp();
    Delay_us(60);
}

//unsigned char wireLoadByte() {
//    unsigned char znak = 0;
//    unsigned char j = 255;
//    for (unsigned char i = 0; i < 8; i++) {
//        znak >>= 1;
//        wirePinOutLog0();
//        Delay_us(2);
//        wirePinInPullUp();
//        Delay_us(5);
//        if (wireGetPin()) {
//            znak |= 0x80;
//            Delay_us(58);
//        } else {
//            while (!wireGetPin())
//                if (!j--) return 0;
//            Delay_us(10);
//        }
//        Delay_us(2);
//    }
//    return znak;
//}
/**
 * Nacita jeden byte z 1wire
 * @return
 */
unsigned char wireLoadByte() {
    unsigned char znak = 0;
    for (unsigned char i = 0; i < 8; i++) {
        znak >>= 1;
        if (wireLoadBite())
            znak |= 0x80;
    }
    return znak;
}

/**
 * Nacita jeden bite z 1wire
 * @return
 */
unsigned char wireLoadBite() {
    wirePinOutLog0();
    Delay_us(2);
    wirePinInPullUp();
    Delay_us(10);//Delay_us(5);
    WIRE_TEST_ON;
    if (wireGetPin()) {
        WIRE_TEST_OFF;
        Delay_us(58);
        return 1;
    }
    WIRE_TEST_OFF;
    while (!wireGetPin());
    return 0;
}

/**
 * Odosle jeden byte na 1wire
 * @param znak
 */
void wireSendByte(unsigned char znak) {
    for (unsigned char i = 0; i < 8; i++) {
        if (znak & 0x01) wireSendLog1();
        else wireSendLog0();
        znak >>= 1;
    }
}

#ifdef WIRE_PARASITE_SUPPLY

/**
 * Pripoji kladne napajanie ku zbernici 1wire
 */
void wirePowerOn() {
    WIRE_DQ_ON;
    WIRE_DQTRIS_ON;
}

/**
 * Odpoji kladne napajanie od zbernici 1wire
 */
void wirePowerOff() {
    WIRE_DQ_OFF;
    WIRE_DQTRIS_OFF;
}
#endif
