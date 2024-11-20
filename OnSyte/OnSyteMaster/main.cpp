/******************************************************************************
* File Name   : main.cpp
* Description : Master half of Onsyte septic control
*			  : mostly setup and state machine
* Author      : Calvin Winey
* Created On  : 11/14/2024
* Version     : see revision.h
* MCU         : <>
* Board       : Raspberry Pi
* Protocol    : UART, SSH
* Compiler    : Visual Studio 19 remote compiled on pi using gcc
* Revision History:
*				see revision.h
* Notes:
*    all files common between Master and Slave extracted and placed in common folder
*
******************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>
#include "main.h"
#include "../Common/revision.h"
#include "../Common/settings.h"
#include "../Common/status.h"
#include "../Common/state.h"
#include "wiringPi.h" // raspi io interface https://github.com/WiringPi
#include "wiringPiI2C.h"
#include "wiringPiSPI.h"
#include "masterHardware.h"


using namespace std;

#define DEMO


// LED Pin - wiringPi pin 0 is BCM_GPIO 17.
// we have to use BCM numbering when initializing with wiringPiSetupSys
// when choosing a different pin number please use the BCM numbering, also
// update the Property Pages - Build Events - Remote Post-Build Event command
// which uses gpio export for setup for wiringPiSetupSys

int main(void)
{
	//int reporting_period;
	unsigned long base_minute_clock;
	unsigned long base_second_clock;


	STATUS status;
	SETTINGS settings;
	STATE_VARS state_vars;

	__time_t loop_status_file_time;
	__time_t state_status_file_time;
	__time_t hourly_status_file_time;
	__time_t pump_status_file_time;
	__time_t aerator_status_file_time;
	__time_t error_status_file_time;


	FileChanged(LOOP_STATUS_FILENAME, &loop_status_file_time);
	FileChanged(STATE_STATUS_FILENAME, &state_status_file_time);
	FileChanged(HOURLY_STATUS_FILENAME, &hourly_status_file_time);
	FileChanged(PUMP_STATUS_FILENAME, &pump_status_file_time);
	//FileChanged( AERATOR_STATUS_FILENAME, &aerator_status_file_time); //combined with pump ststus
	FileChanged(ERROR_STATUS_FILENAME, &error_status_file_time);

	//FileChanged(SETTINGS_FILENAME, &settings_file_time);

	cout << "Revision: " << REVISION << "\n";


	state_vars.current_state = STATE_START;

	wiringPiSetupSys();
	OnsytePiSetup();

	//ActuatorDefaults();
	SettingDefaults(&settings);

#ifdef DEMO
	SettingDemoDefaultsChanges(&settings);
#endif

	//StatusDefaults(&status);
	//StateDefaults(&state_vars);

	PowerUpLightCycle();

	base_minute_clock= clock();
	base_second_clock = clock();

	FILE* ssh = popen("ssh pi@192.168.1.5", "w");


	while (true)
	{
		//sleep(60);//60 second delay -- easy, but ignores execution time
		//SendMsg((char *)"main loop start\n");



		while(clock()<base_minute_clock+ settings.Main_Loop_Time * CLOCKS_PER_SEC)
		{
			while (clock() < base_second_clock +CLOCKS_PER_SEC)//wait 1 second
			{
			}
			base_second_clock = clock();
		//	BlinkStatusLED(state_vars.current_state);
		}
		//SendMsg("main loop clock delay\n");
		base_minute_clock = clock();

		state_vars.current_elapsed++;

		SendMsg("\nMain loop ReadStatus:");
	//	ReadStatus(&status,&settings);

		if (state_vars.status_period > settings.Reporting_Time)
		{
			state_vars.status_period = 0;
		//	SendHourlyStatus();
		}
		state_vars.status_period++;


		if (FileChanged(LOOP_STATUS_FILENAME, &loop_status_file_time))
		{
			SendMsg("\nLoop Status file changed");
		//	testfile = fopen(LOOP_STATUS_FILENAME, "r");
		//	if (testfile != NULL)
		//	{
		//		ReadSettings(testfile, &settings);
		//		fclose(testfile);
		//	}
		}

		if (FileChanged(STATE_STATUS_FILENAME, &state_status_file_time))
		{
			SendMsg("\nState Status file changed");
			//	testfile = fopen(STATE_STATUS_FILENAME, "r");
			//	if (testfile != NULL)
			//	{
			//		ReadSettings(testfile, &settings);
			//		fclose(testfile);
			//	}
		}

		if (FileChanged(HOURLY_STATUS_FILENAME, &hourly_status_file_time))
		{
			SendMsg("\nHourly Status file changed");
			//	testfile = fopen(HOURLY_STATUS_FILENAME, "r");
			//	if (testfile != NULL)
			//	{
			//		ReadSettings(testfile, &settings);
			//		fclose(testfile);
			//	}
		}

		if (FileChanged(PUMP_STATUS_FILENAME, &pump_status_file_time))
		{
			SendMsg("\nPump Status file changed");
			//	testfile = fopen(PUMP_STATUS_FILENAME, "r");
			//	if (testfile != NULL)
			//	{
			//		ReadSettings(testfile, &settings);
			//		fclose(testfile);
			//	}
		}

		if (FileChanged(ERROR_STATUS_FILENAME, &error_status_file_time))
		{
			SendMsg("\nError Status file changed");
			//	testfile = fopen(ERROR_STATUS_FILENAME, "r");
			//	if (testfile != NULL)
			//	{
			//		ReadSettings(testfile, &settings);
			//		fclose(testfile);
			//	}
		}



		SendMsg("main loop State machine ",(unsigned long)state_vars.current_state );

		switch (state_vars.current_state)
		{
			//entry point, really does nothing but move to next state
		case STATE_START:
			//state_vars.current_state++;
			SetState(&state_vars, state_vars.current_state+1);
			break;
			//waits for tank 1 to reach minimum treatment level
			//if so, turn on pump to fill tank 2
	

			//this state does nothing other than being a jump point to exit idle.
			//jumps immediately to start
		case STATE_END:
			//state_vars.current_state = STATE_START;
			SetState(&state_vars, STATE_START);
			break;
		}

		Print_State(&state_vars);
	}
	pclose(ssh);
	return 0;
}

//sets current state and stores elapsed time
void SetState( STATE_VARS *pstate_vars, int state)
{
	pstate_vars->last_state = pstate_vars->current_state;
	pstate_vars->last_elapsed = pstate_vars->current_elapsed;
	pstate_vars->current_state=state;
	pstate_vars->current_elapsed = 0;
}




