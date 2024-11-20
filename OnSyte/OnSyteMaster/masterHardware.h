#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "../Common/settings.h"
#include "../Common/status.h"

#define POWERUP_BLINKRATE 500

//Tank1  //holding tank
//Tank2  //treatment tank
//Tank2  //reactor tank


#define STATUS_LED_PIN	21 //GPIO.21

#define     SPI_CS	8
#define     SPI_MISO    9 //11 // GPIO.ii


void OnsytePiSetup(void);
void PowerUpLightCycle(void);

void BlinkStatusLED(int state);
void EnableStatusLED(bool enable);
void PowerUpLightCycle(void);
bool ReadPump1(void);
bool ReadPump2(void);
bool ReadPump3(void);
bool ReadAerator(void);

