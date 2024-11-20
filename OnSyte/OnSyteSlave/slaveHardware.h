#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "../Common/settings.h"
#include "../Common/status.h"

#define POWERUP_BLINKRATE 500

//Tank1  //holding tank
//Tank2  //digester tank
//Tank2  //reactor tank

#define PUMP1_PIN		16  // Fill pump tank 2 -> tank 3 
#define PUMP2_PIN		26	// Return pump tank 3 -> tank 1
#define PUMP3_PIN	    19	// Discharge pump tank 3 -> discharge
#define AERATOR_PIN		20  // resides in tank 3
#define STATUS_LED_PIN	21 //GPIO.21
#define AD_7193_SYNC_PIN  25


//#define LEVEL1_CHAN   0
#define LEVEL2_CHAN		 0 //1
#define LEVEL3_CHAN		1 //2
//#define LEVEL4_CHAN   3
#define DO_CHAN			4 //3
#define PH_CHAN			5 //4
#define EXT_TEMP_CHAN	6 //5  

#define     SPI_CS	8
#define     SPI_MISO    9 //11 // GPIO.ii

void OnsytePiSetup(void);
void ActuatorDefaults(void);

void EnableDischargePump(bool enable);
void EnableReturnPump(bool enable);
void EnableFillPump(bool enable);
void EnableAerator(bool enable);
void DisableAllActuators(void);
void EnableStatusLED(bool enable);
unsigned long ReadDO(void);
unsigned long ReadPh(void);
float ReadPCBTemp(void);
unsigned long ReadTankTemp(void);
float RawPressureToInchesH2O(unsigned long data);
//unsigned long ReadTank1Pressure(void);
unsigned long ReadTank2PressureRaw(void);
unsigned long ReadTank3PressureRaw(void);
//unsigned int ReadI2cA2D(int port);
unsigned long ReadSpiA2D(int port);
float PressureToLevel(int tank, unsigned long* lookup, float pressure);
//float RawPressureToLevel(int tank, unsigned long* lookup, unsigned long pressure);
float ADCtoReading(unsigned long datam, CALIB calib);
int SPI_Init(unsigned char data1, int data2, unsigned char data3, unsigned char data4);
void SPI_Write(unsigned char data1, unsigned char* data, unsigned char bytesNumber);
void SPI_Read(unsigned char data1, unsigned char* data, unsigned char bytesNumber);
bool ReadSensors(STATUS* pstatus, SETTINGS* psettings);
bool ReadPump1(void);
bool ReadPump2(void);
bool ReadPump3(void);
bool ReadAerator(void);