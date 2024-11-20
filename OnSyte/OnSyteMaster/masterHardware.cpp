/******************************************************************************
* File Name   : masterHardware.cpp
* Description : Master pcb hardware interface functions
*			  : mostly stubs to make common code compatable
* Author      : Calvin Winey
* Created On  : 11/14/2024
* Version     : see revision.h
* MCU         : <>
* Board       : Raspberry Pi
* Protocol    : SPI, I2C, UART, SSH
* Compiler    : Visual Studio 19 remote compiled on pi using gcc
* Revision History:
*				see revision.h
* Notes:
*    all files common between Master and Slave extracted and placed in common folder
*
******************************************************************************/
#include "masterHardware.h"
#include "wiringPi.h"

void OnsytePiSetup(void)
{
	pinMode(STATUS_LED_PIN, OUTPUT);
}

void PowerUpLightCycle(void)
{
	digitalWrite(STATUS_LED_PIN, HIGH);  // On
	delay(POWERUP_BLINKRATE);
	digitalWrite(STATUS_LED_PIN, LOW);	  // Off
}

#define BLINK_PERIOD	1  //on/off/on time
#define NOBLINK_PERIOD 15 // delay between codes


//just stubs, not needed on master unit
bool ReadPump1(void)
{
	return 0;
}
bool ReadPump2(void)
{
	return 0;
}
bool ReadPump3(void)
{
	return 0;
}
bool ReadAerator(void)
{
	return 0;
}

//turns status led off/on
void EnableStatusLED(bool enable)
{
//	if (enable)
//		digitalWrite(STATUS_LED_PIN, HIGH);
//	else
//		digitalWrite(STATUS_LED_PIN, LOW);
}