/******************************************************************************
* File Name   : slaveHardware.cpp
* Description : Slave half of Onsyte septic control
*			  : hardware interface ( pumps, sensors, led )
* Author      : Calvin Winey
* Created On  : 11/14/2024
* Version     : see revision.h
* MCU         : <>
* Board       : Raspberry Pi
* Protocol    : SPI, I2C,  
* Compiler    : Visual Studio 19 remote compiled on pi using gcc
* Revision History:
*				see revision.h
* Notes:
*    all files common between Master and Slave extracted and placed in common folder
*	hardware using third party wiringPi libraries
*
******************************************************************************/
#include "slaveHardware.h"
#include "wiringPi.h"
#include "wiringPiI2C.h"
#include "wiringPiSPI.h"
#include "AD7193.h"
#include <stdio.h>
#include <iostream>
#include <string.h>

using namespace std;

//setup i/o pins
void OnsytePiSetup(void)
{
	pinMode(PUMP1_PIN, OUTPUT);
	pinMode(PUMP2_PIN, OUTPUT);
	pinMode(PUMP3_PIN, OUTPUT);
	pinMode(AERATOR_PIN, OUTPUT);
	pinMode(STATUS_LED_PIN, OUTPUT);
	pinMode(AD_7193_SYNC_PIN, OUTPUT);
	digitalWrite(AD_7193_SYNC_PIN, HIGH);  // On

}



//sets all actuators to default (off state)
void ActuatorDefaults(void)
{
	EnableDischargePump(false);
	EnableReturnPump(false);
	EnableFillPump(false);
	EnableAerator(false);
}




//turns discharge pump off/on
void EnableDischargePump(bool enable)
{
if(enable)
	digitalWrite(PUMP3_PIN, HIGH);
else
	digitalWrite(PUMP3_PIN, LOW);
}
//turns return pump off/on
void EnableReturnPump(bool enable)
{
	if (enable)
		digitalWrite(PUMP2_PIN, HIGH);
	else
		digitalWrite(PUMP2_PIN, LOW);
}
//turns Fill pump off/on
void EnableFillPump(bool enable)
{
	if (enable)
		digitalWrite(PUMP1_PIN, HIGH);
	else
		digitalWrite(PUMP1_PIN, LOW);
}
//turns aerator off/on
void EnableAerator(bool enable)
{
	if (enable)
		digitalWrite(AERATOR_PIN, HIGH);
	else
		digitalWrite(AERATOR_PIN, LOW);
}
//turns status led off/on
void EnableStatusLED(bool enable)
{
	if (enable)
		digitalWrite(STATUS_LED_PIN, HIGH);
	else
		digitalWrite(STATUS_LED_PIN, LOW);
}
//turns off all actuators
void DisableAllActuators(void)
{
	digitalWrite(PUMP1_PIN, LOW);
	digitalWrite(PUMP2_PIN, LOW);
	digitalWrite(PUMP3_PIN, LOW);
	digitalWrite(AERATOR_PIN, LOW);
}
//reads disolved oxygen sensor
unsigned long ReadDO(void)
{
	unsigned long data;

	AD7193_RangeSetup(1, AD7193_CONF_GAIN_1); // Unipolar operation, 0 Gain.
	AD7193_Pseudo(1);
	data = ReadSpiA2D(DO_CHAN);

	return data;
}
//reads ph sensor
unsigned long ReadPh(void)
{
	unsigned long data;


	AD7193_RangeSetup(1, AD7193_CONF_GAIN_1); // Unipolar operation, 0 Gain.
	AD7193_Pseudo(1);
	data = ReadSpiA2D(PH_CHAN);

	return data ;

}
//reads temperature sensor
float ReadPCBTemp(void)
{
	float data;

	data = AD7193_TemperatureRead(); // ReadSpiA2D(TEMP_I2C);

	return data;
}
//reads temperature sensor
unsigned long ReadTankTemp(void) //no tank 1 level
{
	unsigned long data;
	MyWait("AD7193_RangeSetup");
	AD7193_RangeSetup(1, AD7193_CONF_GAIN_1); // Unipolar operation, 0 Gain.
	AD7193_Pseudo(1);

	data = ReadSpiA2D(EXT_TEMP_CHAN);

	return data;
}

//reads raw tank 1 pressure
/*unsigned long ReadTank1Pressure(void) //no tank 1 level
{
	unsigned long data;
	MyWait("AD7193_RangeSetup");
	AD7193_RangeSetup(1, AD7193_CONF_GAIN_1); // Unipolar operation, 0 Gain.
	AD7193_Pseudo(1);

	data = ReadSpiA2D(LEVEL1_CHAN);

	return data;
}*/
//reads raw tank 2 pressure
float RawPressureToInchesH2O(unsigned long data)
{
	//16777215 counts = 83 inH20
	return (float)data / 202135;
}
unsigned long ReadTank2PressureRaw(void)
{
	unsigned long data;
	MyWait("AD7193_RangeSetup");
	AD7193_RangeSetup(1, AD7193_CONF_GAIN_1); // Unipolar operation, 0 Gain.
	AD7193_Pseudo(1);

	data = ReadSpiA2D(LEVEL2_CHAN);

	return data;
}
//reads raw tank 3 pressure
unsigned long ReadTank3PressureRaw(void)
{
	unsigned long data;
	MyWait("AD7193_RangeSetup");
	AD7193_RangeSetup(1, AD7193_CONF_GAIN_1); // Unipolar operation, 0 Gain.
	AD7193_Pseudo(1);

	data = ReadSpiA2D(LEVEL3_CHAN);

	return data;
}
//I2C A2D read function
//unsigned int ReadI2cA2D(int port)
//{
//	int fd;
//	int data;
//	data = 0;
//	SendMsg("ReadI2cA2D1");
//	//fd = wiringPiI2CSetup(port);
//	fd = wiringPiI2CSetupInterface("/dev/i2c-3", port); // 0x31);
//	if (fd < 0)
//	{
//		//error
//	}
//	SendMsg("ReadI2cA2D2");
//	data = wiringPiI2CRead(fd);
//
//	if (data < 0)
//	{
//		//error
//	}
//	return data;
//}

/*import wiringpi
SPIchannel = 1 #SPI Channel(CE1)
SPIspeed = 500000 #Clock Speed in Hz
wiringpi.wiringPiSetupGpio()
wiringpi.wiringPiSPISetup(SPIchannel, SPIspeed)

sendData = str(42) #will send TWO bytes, a byte 4 and a byte 2
recvData = wiringpi.wiringPiSPIDataRW(SPIchannel, sendData)
#recvData now holds a list[NumOfBytes, recvDataStr] e.g.[2, '\x9A\xCD']

#alternatively, to send a single byte :
sendData = chr(42) #will send a single byte containing 42
recvData = wiringpi.wiringPiSPIDataRW(SPIchannel, sendData)
#recvData is again a list e.g.[1, '\x9A']*/


//read a single word from the a2d
unsigned long ReadSpiA2D(int port)
{
	//select a port
	AD7193_ChannelSelect(port);
	//allow settling
	delay(10);

	//SendMsg("ReadSpiA2D - AD7193_SingleConversion");
	return AD7193_SingleConversion();
}


//converts pressure in inches of water to level in gallons
//index is pressure, table values are gallons
float PressureToLevel(int tank, unsigned long *lookup, float raw_pressure)
{
int index;
float scale;
float delta;

index=(int) raw_pressure;
scale=raw_pressure - (float)index;

if(index >=49)
	return lookup[49];

	delta=lookup[index+1]-lookup[index];

return (float)lookup[index]+delta*scale;
}

/*
//converts pressure (A2D value) to level (gallons)
//assumes table 50 elements where the index is inches and the data is raw A2D  pressure value at that level
float RawPressureToLevel(int tank, unsigned long *lookup, unsigned long pressure)
{
	unsigned long high;
	unsigned long low;
	float data;
	float fraction;
	int index;

	//handle possible underrun
	if (pressure <= lookup[0])
		return 0.0;

	fraction = 1;
	low = 0.0;
	high = 0.0;
	index = 0;

	//set thru the lookup table, low to high, finding the 2 closest entries
	for (int i = 0; i < 50; i++)
	{
		if (pressure >= lookup[i])
		{
			high = lookup[i];
			if(i>0)
				low = lookup[i-1];
			index = i;
			
		}
		else
		{
		//cout << " index " << index << " high " << high << " low " << low;
			break;
		}
	}

	if (high == 0)
	{
		high = lookup[49];
		low = lookup[49];
		index = 49;
	}
	if(high==low)
		fraction = ((float)pressure - (float)low);
	else
		fraction =  ((float)pressure- (float)low)/((float)high- (float)low);
	data = (float)index-1.0 + fraction;
	if (data < 0)
		data = 0;
	//cout << " index " << index << " high " << high << " low " << low;
	//cout << " data = " << data << "\n";
	return 40.0 * data; //scale so that full scale = 2000
}
*/
float ADCtoReading(unsigned long datam, CALIB calib)
{
	return ((float)datam - (float)calib.Offset) / (float)calib.Scale  ;
}

#define channel 0

void SPI_Write(unsigned char data1, unsigned char* data, unsigned char bytesNumber)
{
	wiringPiSPIDataRW(channel, data, bytesNumber);
}
void SPI_Read(unsigned char data1, unsigned char* data, unsigned char bytesNumber)
{
	wiringPiSPIDataRW(channel, data, bytesNumber);
}
#define SPI_CPHA 0x01
#define SPI_CPOL 0x02
#define SPI_READY 0x80

int SPI_Init(unsigned char data1, int data2, unsigned char data3, unsigned char data4)
{
	int status;

	//status = wiringPiSPISetupMode(channel, 100000, SPI_READY | 3);
	status= wiringPiSPISetupMode(channel, 100000, SPI_READY| SPI_CPHA| SPI_CPOL);
	SendMsg("wiringPiSPISetup", (unsigned long) status);
	return true;
}

//reads all sensors and converts from adc values to real values
bool ReadSensors(STATUS* pstatus, SETTINGS* psettings)
{
	//pstatus->tank1_pressure = RawPressureToInchesH2O(ReadTank1Pressure());
	//pstatus->tank1_level = PressureToLevel(1, psettings->Tank_Level_Lookup, ReadTank1Pressure());
	//SendMsg("tank1_pressure (in)", pstatus->tank1_pressure);
	//SendMsg("tank1_level (gal)", pstatus->tank1_level);

	SendMsg("");
	pstatus->tank2_pressure = RawPressureToInchesH2O(ReadTank2PressureRaw());
	pstatus->tank2_level = PressureToLevel(2, psettings->Tank2_Level_Lookup, pstatus->tank2_pressure);
	//SendMsg("tank2_pressure (raw)", ReadTank2PressureRaw());
	SendMsg("tank2_pressure (inH2O)", pstatus->tank2_pressure);
	SendMsg("tank2_level (gal)", pstatus->tank2_level);
	SendMsg("");

	pstatus->tank3_pressure = RawPressureToInchesH2O(ReadTank3PressureRaw());
	pstatus->tank3_level = PressureToLevel(3, psettings->Tank3_Level_Lookup, pstatus->tank3_pressure);
	//SendMsg("tank3_pressure (raw)", ReadTank3PressureRaw());
	SendMsg("tank3_pressure (inH2O)", pstatus->tank3_pressure);
	SendMsg("tank3_level (gal)", pstatus->tank3_level);
	SendMsg("");

	pstatus->oxygen = ADCtoReading(ReadDO(), psettings->DO_Cal);
	SendMsg("oxygen", pstatus->oxygen);
	pstatus->ph = ADCtoReading(ReadPh(), psettings->Ph_Cal);
	SendMsg("ph", pstatus->ph);
	pstatus->pcb_temp = ReadPCBTemp();
	SendMsg("PCB temperature", pstatus->pcb_temp);
	pstatus->tank_temp = ADCtoReading(ReadTankTemp(), psettings->T_Cal);
	SendMsg("Tank temperature", pstatus->tank_temp);

	return true;
}

bool ReadPump1(void)
{
	return digitalRead(PUMP1_PIN);
}
bool ReadPump2(void)
{
	return digitalRead(PUMP2_PIN);
}
bool ReadPump3(void)
{
	return digitalRead(PUMP3_PIN);
}
bool ReadAerator(void)
{
	return digitalRead(AERATOR_PIN);
}

