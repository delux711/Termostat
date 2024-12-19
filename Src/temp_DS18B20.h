/* 
 * File:   temp_DS18B20.h
 * Author: delux
 *
 * Created on Sobota, 2013, august 31, 17:01
 *
 * Hodnoty od vyroby:
 * Teplota 0x5005
 * UserData 0x4b46
 * Config 0x7f
 * CRC 0x1c
 */

#ifndef TEMP_DS18B20_H
#define	TEMP_DS18B20_H
#include "1wire.h"

#define defConfig9bit 0x1F
#define defConfig10bit 0x3F
#define defConfig11bit 0x5F
#define defConfig12bit 0x7F

#define TEMP_POLE_ROM
//#define WIRE_PARASITE_SUPPLY
#define WIRE_EXTERNAL_SUPPLY
#define TEMP_STRUCT_ROM

#ifdef TEMP_STRUCT_ROM
typedef struct {
    unsigned char familyCode;
    unsigned char serialNum[6];
    unsigned char crc;
} ROM;
#endif
typedef struct {
    unsigned int teplota;
    unsigned char userByteTh;
    unsigned char userByteTl;
    unsigned char configByte;
    unsigned char crc;
} TEPLOMER;

enum enTeplPole {
    enTeplPoleFamilyCode, enTeplPoleSerialNumber0, enTeplPoleSerialNumber1,
    enTeplPoleSerialNumber2, enTeplPoleSerialNumber3, enTeplPoleSerialNumber4,
    enTeplPoleSerialNumber5, enTeplPoleSerialNumber6, enTeplPoleCRC
};

#ifdef TEMP_STRUCT_ROM
ROM tempLoadRom();
#else //TEMP_POLE_ROM
//unsigned char *tempLoadRom();
#endif
void tempInic();
void tempRemoveRoms();
unsigned char iButtonHladaj(unsigned char *serialNumber);
void tempSetDefaultTemp(unsigned char *romTemp);
unsigned char tempRomSkip();
unsigned char tempRomMatch(unsigned char *adresa);
unsigned char *tempRomLoad();
void tempRomSearch(unsigned char alarm);
unsigned char tempReadPowerSupply();
//unsigned char tempCRCverify(unsigned char *data, unsigned char pocet);
unsigned char tempCRCverify(unsigned char pocet);
void tempStartConvert();
TEPLOMER tempLoadData();
void tempSendData(unsigned char tH, unsigned char tL, unsigned char config);
void tempSendDataEEPROM(unsigned char tH, unsigned char tL, unsigned char defConfig);
void tempSendDefault();
unsigned char *tempLoadTempString();
unsigned char tempPocetZariadeni();
unsigned char *temp1();
unsigned char *temp2();
unsigned char *temp3();
unsigned char *temp4();
unsigned char *temp5();
//unsigned char *wireLoadSerialNumber();
//unsigned char wireLoadFamilyCode();
#endif	/* TEMP_DS18B20_H */

