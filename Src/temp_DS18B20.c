#include "temp_DS18B20.h"
#include "1wire.h"
//#include "comunication/uartAsynch300bit.h"
#include <stdio.h>

#ifdef TEMP_STRUCT_ROM
ROM r1;
#else //TEMP_POLE_ROM
#endif
void tempPowerOn(unsigned char prikaz);
unsigned char tempInicTeplomer();

TEPLOMER tepl;
unsigned char tempPower = 0x01; // nepripojene ziadne zariadenie 0x01
unsigned char tempData[9];
unsigned char tempRoms1[8];
unsigned char tempRoms2[8];
unsigned char tempRoms3[8];
unsigned char tempRoms4[8];
unsigned char tempRoms5[8];
unsigned char tempString[8];
unsigned char tempPocet;
unsigned char *tempRoms;

void tempRemoveRoms() {
    unsigned char i;
    for (i = 0; i < 8; i++)
        tempRoms1[i] = 0xFF;
    for (i = 0; i < 8; i++)
        tempRoms2[i] = 0xFF;
    for (i = 0; i < 8; i++)
        tempRoms3[i] = 0xFF;
    for (i = 0; i < 8; i++)
        tempRoms4[i] = 0xFF;
    for (i = 0; i < 8; i++)
        tempRoms5[i] = 0xFF;
}

unsigned char tempInicTeplomer() {
    if (tempRoms1[0] == 0x28)
        tempRoms = tempRoms1;
    else if (tempRoms2[0] == 0x28)
        tempRoms = tempRoms1;
    else if (tempRoms3[0] == 0x28)
        tempRoms = tempRoms1;
    else if (tempRoms4[0] == 0x28)
        tempRoms = tempRoms1;
    else if (tempRoms5[0] == 0x28)
        tempRoms = tempRoms1;
    else
        return 0;
    return 1;
}

void tempInic() {
    tempRomSearch(0);
	/*
    if(!tempInicTeplomer())
        lcdGsendStringChyba("Nenajdeny\ntepelny\nsenzor!", 9);*/
}

/**
 * Zisti ci sa na zbernici nachadza aspon 1 iButton klucenka
 * @param serialNumber pokial sa nachadza naplni ho seriovym cislom
 * @return 1-ano, 0-nie
 */
unsigned char iButtonHladaj(unsigned char *serialNumber) {
    unsigned char i;
    tempRomSearch(0);
    serialNumber[0] = 0xFF;
    serialNumber[7] = 0;
    if (tempRoms1[0] == 0x01 || !tempRoms1[0])
        for (i = 0; i < 8; i++)
            serialNumber[i] = tempRoms1[i];
    else if (tempRoms2[0] == 0x01 || !tempRoms2[0])
        for (i = 0; i < 8; i++)
            serialNumber[i] = tempRoms2[i];
    else if (tempRoms3[0] == 0x01 || !tempRoms3[0])
        for (i = 0; i < 8; i++)
            serialNumber[i] = tempRoms3[i];
    else if (tempRoms4[0] == 0x01 || !tempRoms4[0])
        for (i = 0; i < 8; i++)
            serialNumber[i] = tempRoms4[i];
    else if (tempRoms5[0] == 0x01 || !tempRoms5[0])
        for (i = 0; i < 8; i++)
            serialNumber[i] = tempRoms5[i];
    tempInicTeplomer();
    if (serialNumber[0] == 0x01 || !serialNumber[0])
        if (serialNumber[7])
            return 1;
    return 0;
}

void tempSetDefaultTemp(unsigned char *romTemp) {
    tempRoms = romTemp;
}

#ifdef TEMP_STRUCT_ROMm

ROM tempRomLoad() {
    unsigned char j = 0;
    if (wireInitBus()) {
        r1.crc = 0;
        return r1;
    }
    wireSendByte(0x33);
    tempData[0] = wireLoadByte();
    r1.familyCode = tempData[0];
    for (unsigned char i = 0; i < 6; i++)
        r1.serialNum[i] = wireLoadByte();
    r1.crc = wireLoadByte();
    wireSendByte(0);
    return r1;
}
#else //TEMP_POLE_ROM

/**
 * Nacita ROM data z teplomera do pola
 * @return 1 - nenaviazala sa komunikacia
 * @return 2 - prijaly sa nevalidne data
 * @return * - smernik na ulozene validne hodnoty
 * @return d[enTeplPoleFamilyCode] - family code
 * @return d[enTeplPoleSerialNumber0] - d[enTeplPoleSerialNumber6] - serial number
 * @return d[enTeplPoleCRC] - crc
 */
unsigned char *tempRomLoad() {
    unsigned char j = 0, d;
    do {
        if (wireInitBus() || j > 9)
            return 0;
        j++;
        wireSendByte(0x33);
        for (unsigned char i = 0; i < 8; i++)
            tempData[i] = wireLoadByte();
    } while (tempCRCverify(7) != tempData[7]);
    return tempData;
}
#endif

/**
 * 
 * @param adresa
 * @return
 */
unsigned char tempRomMatch(unsigned char *adresa) {
    unsigned char adr;
    if (wireInitBus())
        return 1;
    wireSendByte(0x55);
    for (unsigned char i = 0; i < 8; i++) {
        adr = adresa[i];
        for (unsigned char j = 0; j < 8; j++) {
            if (adr & 0x01) wireSendLog1();
            else wireSendLog0();
            adr >>= 1;
        }
    }
    return 0;
}

/**
 * Na nasledujuci prikaz budu reagovat vsetky pripojene zariadenia
 * @return
 */
unsigned char tempRomSkip() {
    if (wireInitBus())
        return 1;
    wireSendByte(0xCC);
    return 0;
}

/**
 * Zisti ci je zariadenie napajane parazitnym napajanim
 * @return 0x00 - parazitne, 0xFF - normalne Vcc, 0x01 - nepripojene ziadne zariadenie
 */
unsigned char tempReadPowerSupply() {
    /*
    if (wireInitBus())
        return 1;
    wireSendByte(0xCC);
     */
    wireSendByte(0xB4);
    tempPower = wireLoadByte();
    return tempPower;
}

//unsigned char tempCRCverify(unsigned char *data, unsigned char pocet) {

unsigned char tempCRCverify(unsigned char pocet) {
    unsigned char shift_reg = 0, data_bit;
    for (unsigned char i = 0; i < pocet; i++) {
        //data_bit = data[i];
        data_bit = tempData[i];
        for (unsigned char j = 0; j < 8; j++) {
            if (((data_bit & 0x01) ^ (shift_reg & 0x01)) & 0x01) {
                shift_reg >>= 1;
                shift_reg ^= 0x8c;
            } else shift_reg >>= 1;
            data_bit >>= 1;
        }
    }
    return (shift_reg);

}

/**
 * Odosle prikaz do teplomeru. V pripade parazitneho napajania zapne toto napajanie
 * Pri parazitnom napajani po odoslani prikazu caka 800ms
 * @param prikaz
 */
void tempPowerOn(unsigned char prikaz) {
    /*
    if (wireInitBus())
        return;
    wireSendByte(0xCC);
     */
#ifdef WIRE_PARASITE_SUPPLY
    if (tempPower == 0) {
        wireSendByte(prikaz);
        wirePowerOn();
        for (unsigned char i = 0; i < 8; i++) {
            __delay_ms(25);
            __delay_ms(25);
            __delay_ms(25);
            __delay_ms(25);
        }
        wirePowerOff();
        return;
    }
#endif
#ifdef WIRE_EXTERNAL_SUPPLY
    wireSendByte(prikaz);
    //while (!wireLoadByte());
#endif
}

void tempStartConvert() {
    tempRomMatch(tempRoms);
    tempPowerOn(0x44);
}

/**
 * Nastavi uzivatelske data a configuracny register (presnost merania)
 * @param tH
 * @param tL
 * @param defConfig - pouzivat defConfig9bit az defConfig12bit
 */
void tempSendData(unsigned char tH, unsigned char tL, unsigned char defConfig) {
    /*
    if (wireInitBus())
        return;
    wireSendByte(0xCC);
     */
    wireSendByte(0x4E);
    wireSendByte(tH);
    wireSendByte(tL);
    wireSendByte(defConfig);
}

/**
 * Ulozi uzivatelske data a configuracny register (presnost merania) do pamete EEPROM
 * @param tH
 * @param tL
 * @param defConfig - pouzivat defConfig9bit az defConfig12bit
 */
void tempSendDataEEPROM(unsigned char tH, unsigned char tL, unsigned char defConfig) {
    tempSendData(tH, tL, defConfig);
    tempPowerOn(0x48);
}

/**
 * Presunie uziv.data a config.reg z EEPROM do RAM
 */
void tempSendDefault() {
    /*
    if (wireInitBus())
        return;
    wireSendByte(0xCC);
     */
    wireSendByte(0xB8);
    while (!wireLoadByte());
    return;
}

TEPLOMER tempLoadData() {
    unsigned char j = 0;
    tempRomMatch(tempRoms);
    do {
        if (9 < j++) {
            tepl.crc = 1;
            break;
        }
        wireSendByte(0xBE);
        for (unsigned char i = 0; i < 9; i++) {
            tempData[i] = wireLoadByte();
        }
        tepl.teplota = tempData[1];
        tepl.teplota <<= 8;
        tepl.teplota |= tempData[0];
        tepl.userByteTh = tempData[2];
        tepl.userByteTl = tempData[3];
        tepl.configByte = tempData[4];
        tepl.crc = tempData[8];
    } while (tempCRCverify(8) != tepl.crc);
    return tepl;
}

unsigned char *tempLoadTempString() {
    unsigned char i, j;
    unsigned char cele, desatinne;
    unsigned int desatI = 0;
    i = 0;
    //tempStartConvert();
    if (wireLoadByte())
        tempLoadData();
    if ((cele = tempData[1]) > 0x07) {
        tempString[i++] = '-'; // som v minusovych cislach
        cele &= 0x07;
        cele = ~cele;
        cele++;
        desatinne = ~tempData[0];
        desatinne++;
    } else desatinne = tempData[0];
    cele <<= 4;
    cele |= (desatinne & 0xF0) >> 4;
    desatinne &= 0x0F;
    j = 0;
    while (cele > 99) {
        j++;
        cele -= 100;
    }
    if (j != 0)
        tempString[i++] = j + '0';
    j = 0;
    while (cele > 9) {
        j++;
        cele -= 10;
    }
    tempString[i++] = j + '0';
    tempString[i++] = cele + '0';
    tempString[i++] = ',';
    if (desatinne) {
        if (desatinne & 0x08)
            desatI = 5000;
        if (desatinne & 0x04)
            desatI += 2500;
        if (desatinne & 0x02)
            desatI += 1250;
        if (desatinne & 0x01)
            desatI += 625;
        j = 0;
        while (desatI > 999) {
            j++;
            desatI -= 1000;
        }
        tempString[i++] = j;
        j = 0;
        while (desatI > 99) {
            j++;
            desatI -= 100;
        }
        tempString[i++] = j;
        desatinne = (unsigned char) desatI;
        j = 0;
        while (desatinne > 9) {
            j++;
            desatinne -= 10;
        }
        tempString[i++] = j;
        tempString[i++] = desatinne;
        tempString[i] = '\0';
        for (j = 0; j < 4; j++)
            tempString[--i] += '0';
        i += 3;
        while (tempString[i] == '0')
            tempString[i--] = ' ';
    } else {
        tempString[i++] = '0';
        tempString[i] = '\0';
    }
    return tempString;
}

/**
 * Najde vsetky rom na zbernici (maximalne 5), podla poctu X - tempRomsX[8];
 * @param alarm - 0 vracia room, 1 vracia alarm
 */
void tempRomSearch(unsigned char alarm) {
    unsigned char i, j, z1, z2, pocet, zalI, zalJ;
    unsigned char hladane;
    tempRemoveRoms();
    pocet = 0;
    zalI = 0xFF;
    zalJ = 0xFF;
    while (pocet < 5) {
        if (wireInitBus())
            return;
        if (alarm) wireSendByte(0xEC);
        else wireSendByte(0xF0);
        Delay_us(500);
        for (i = 0; i < 8; i++) {
            hladane = 0;
            for (j = 0; j < 8; j++) {
                z1 = wireLoadBite();
                z2 = wireLoadBite();
                if (z1 ^ z2) {
                    hladane >>= 1;
                    if (z1) {
                        wireSendLog1();
                        hladane |= 0x80;
                    } else
                        wireSendLog0();
                } else if (z1 == 0 && z2 == 0) {
                    hladane >>= 1;
                    if (zalI == 0xFF && zalJ == 0xFF) {
                        zalI = i;
                        zalJ = j;
                        wireSendLog0();
                    } else if (zalI == i && zalJ == j) {
                        wireSendLog1();
                        hladane |= 0x80;
                        zalI = 0xFF;
                        zalJ = 0xFF;
                    } else wireSendLog0();
                }
            }
            if (pocet == 0) tempRoms1[i] = hladane;
            else if (pocet == 1) tempRoms2[i] = hladane;
            else if (pocet == 2) tempRoms3[i] = hladane;
            else if (pocet == 3) tempRoms4[i] = hladane;
            else if (pocet == 4) tempRoms5[i] = hladane;
        }
        pocet++;
    }
}

unsigned char *temp1() {
    return tempRoms1;
}

unsigned char *temp2() {
    return tempRoms2;
}

unsigned char *temp3() {
    return tempRoms3;
}

unsigned char *temp4() {
    return tempRoms4;
}

unsigned char *temp5() {
    return tempRoms5;
}

unsigned char tempPocetZariadeni() {
    return tempPocet;
}

//unsigned char **tempSearchRom() {
//    unsigned char i, j, z, z1, z2, pocet, pocetB, koniec = 1;
//    unsigned char hladane = 0;
//    unsigned char najdene[8];
//    for (unsigned char i = 0; i < 8; i++) {
//        najdene[i] = 0;
//        tempData[i] = 0;
//    }
//    tempPocet = 0;
//    pocet = 0xFF;
//    pocetB = 0;
//    z = 0;
//    while (pocet) {
//        if (wireInitBus())
//            return 0;
//        wireSendByte(0xF0);
//        for (i = 0; i < 8; i++) {
//            hladane = 0;
//            for (j = 0; j < 8; j++) {
//                z1 = wireLoadBite();
//                z2 = wireLoadBite();
//                if (z1 ^ z2) {
//                    hladane >>= 1;
//                    if (z1) {
//                        wireSendLog1();
//                        hladane |= 0x80;
//                    } else
//                        wireSendLog0();
//                } else if (z1 == 0 && z2 == 0) {
//                    if (pocet == 0xFF)
//                        pocet = 0;
//                    hladane >>= 1;
//                    if ((pocet >> pocetB) & 0x01) {
//                        wireSendLog1();
//                        hladane |= 0x80;
//                    } else {
//                        wireSendLog0();
//                    }
//                    pocetB++;
//                } else if (z1 && z2) {
//                    tempPocet = pocet;
//                    return &tempRoms;
//                }
//            }
//            tempRoms[z][i] = hladane;
//        }
//        z++;
//        pocetB--;
//        if ((pocet >> pocetB) & 0x01) {
//            pocet |= 0 << pocetB;
//            pocetB--;
//        }
//        pocet |= 1 << pocetB;
//        pocetB = 0;
//    }
//    tempPocet = z;
//    return &tempRoms;
//}
