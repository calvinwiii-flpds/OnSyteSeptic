/******************************************************************************
* File Name   : main.cpp
* Description : Slave half of Onsyte septic control
*			  : mostly setup and state machine
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

#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <iostream>
#include "main.h"
#include "../Common/revision.h"

#include "wiringPi.h" // raspi io interface https://github.com/WiringPi
#include "wiringPiI2C.h"
#include "wiringPiSPI.h"
#include "slaveHardware.h"
#include "AD7193.h"

//#include "..\Common\status.h"
//#include "..\Common\settings.h"
//#include "..\Common\state.h"
//#include "..\Common\error.h"

using namespace std;

//#define DEMO
//#define WRITE_DEFAULT_SETTINGS
#define UNSUPPORTED_ERROR        //undefine to pass all errors		


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
	unsigned int overtemp_count;
	__time_t settings_file_time;

	STATUS status;
	SETTINGS settings;
	STATE_VARS state_vars;
	ERROR_ENTRY errors[MAX_ERRORS];

	bool have_error;
	//bool do_abort_restart;
	//bool do_next;

	cout << "Revision: " << REVISION << "\n";

	ClearAllErrors(errors);

	state_vars.current_state = STATE_START;
	overtemp_count = 0;

	wiringPiSetupSys();
	OnsytePiSetup();

	MyWait("AD7193_Init");

	AD7193_Init();

	AD7193_Reset();

	ActuatorDefaults();

	SettingDefaults(&settings);


#ifdef DEMO
	SettingDemoDefaultsChanges(&settings);

#endif

	StatusDefaults(&status);
	StateDefaults(&state_vars);

	FILE* testfile;


 #ifdef WRITE_DEFAULT_SETTINGS
#ifdef DEMO
	testfile = fopen("settings_dbg.txt", "w");
#else
	testfile = fopen("settings_std.txt", "w");
#endif
	//testfile = fopen("testfile.txt", "w");
	WriteSettings(testfile, &settings);
	fclose( testfile);

	#endif
	/* settings read write test
	SettingCleared(&settings);

	testfile = fopen("testfile.txt", "r");
	ReadSettings(testfile, &settings);
	fclose(testfile);

	testfile = fopen("testfile2.txt", "w");
	WriteSettings(testfile, &settings);
	fclose(testfile);
	*/

	//testfile = fopen("settings.txt", "r");
	testfile = fopen(SETTINGS_FILENAME, "r");
	if(testfile!=NULL)
	{
	//	struct stat filestat;

		ReadSettings(testfile, &settings);
		fclose(testfile);
	}
	else
	{
		SendMsg("No Settings.txt file\n");
	}

	FileChanged(SETTINGS_FILENAME, &settings_file_time);


#ifdef DEMO
	PowerUpLightCycle();
#endif

	//since we now have fatal errors, do a couple reads to make sure adc buffers are clear 
	//else we may have an accidental fatal error
	ReadSensors(&status, &settings);
	sleep(1);
	ReadSensors(&status, &settings);
	sleep(1);

	base_minute_clock= clock();
	base_second_clock = clock();

	while (true)
	{
		//do_abort_restart = false;
		//do_next = false;
		//sleep(60);//60 second delay -- easy, but ignores execution time
		//SendMsg((char *)"main loop start\n");

		while(clock()<base_minute_clock+ settings.Main_Loop_Time * CLOCKS_PER_SEC)
		{
			while (clock() < base_second_clock +CLOCKS_PER_SEC)//wait 1 second
			{
			}
			base_second_clock = clock();
			BlinkStatusLED(state_vars.current_state);
		}
		//SendMsg("main loop clock delay\n");
		base_minute_clock = clock();	struct stat attrib;


		if (FileChanged(SETTINGS_FILENAME, &settings_file_time))
		{
			SendMsg("\nNew Settings file");
			testfile = fopen(SETTINGS_FILENAME, "r");
			if (testfile != NULL)
			{
				ReadSettings(testfile, &settings);
				fclose(testfile);
			}
		}


		SendMsg("\nMain loop ReadStatus:");
		ReadSensors(&status,&settings);

		//checking fatal errors
		have_error = status.pcb_temp > settings.CPU_Over_Temp;   //cpu over temp
		if (have_error)
		{
			SetError(errors, ERROR_CPU_TEMP);
			SetState(&state_vars, errors[ERROR_CPU_TEMP].exit_state);
		}
		else
			ClearError(errors, ERROR_CPU_TEMP);
			
#ifndef UNSUPPORTED_ERROR
		have_error = status.tank_temp > settings.Tank_Over_Temp;  //tank over temp
		if (have_error)
		{
			SetError(errors, ERROR_TANK_TEMP);
			SetState(&state_vars, errors[ERROR_TANK_TEMP].exit_state);
		}
		else
			ClearError(errors, ERROR_TANK_TEMP);
#endif
		have_error = status.tank2_pressure > settings.Tank2_Sensor_Limit; //invalid tank 2 sensor
		if (have_error)
		{
			SetError(errors, ERROR_LEVEL_2);
			SetState(&state_vars, errors[ERROR_LEVEL_2].exit_state);
		}
		else
			ClearError(errors, ERROR_LEVEL_2);

		have_error = status.tank3_pressure > settings.Tank3_Sensor_Limit; //invalid tank 3 sensor
		if (have_error)
		{
			SetError(errors, ERROR_LEVEL_3);
			SetState(&state_vars, errors[ERROR_LEVEL_3].exit_state);
		}
		else
			ClearError(errors, ERROR_LEVEL_3);

		//if no fatal error, check global non-fatal errors
		if (state_vars.current_state != STATE_ABORT)
		{
			have_error = (status.tank2_pressure > settings.Tank2_High_Limit) &&  //washout condition
						(status.tank3_pressure > settings.Tank3_High_Limit);
			if (have_error)
			{
				SetError(errors, ERROR_WASHOUT);
				SetState(&state_vars, errors[ERROR_WASHOUT].exit_state);
				ClearError(errors, ERROR_TANK_2);
				ClearError(errors, ERROR_TANK_3);
			}
			else
				ClearError(errors, ERROR_WASHOUT);
			
			// if not a washout, check tanks seperately
			if (!have_error)
			{
				have_error = status.tank2_pressure > settings.Tank2_High_Limit; //tank 2 overfull
				if (have_error)
				{
					SetError(errors, ERROR_TANK_2);
					SetState(&state_vars, errors[ERROR_TANK_2].exit_state);
				}
				else
					ClearError(errors, ERROR_TANK_2);

				have_error = status.tank3_pressure > settings.Tank3_High_Limit; //tank 3 overfull
				if (have_error)
				{
					SetError(errors, ERROR_TANK_3);
					SetState(&state_vars, errors[ERROR_TANK_3].exit_state);
				}
				else
					ClearError(errors, ERROR_TANK_3);
			}
		}

		state_vars.status_period++;

		SendMsg("main loop State machine ",(unsigned long)state_vars.current_state );

		switch (state_vars.current_state)
		{
		//entry point, really does nothing but move to next state
		//disables actuators incase we got here from an error
		case STATE_START:
			SetState(&state_vars, state_vars.current_state+1);
			DisableAllActuators();
			break;

		//removes excess from tank 3 if not at initial conditions
		case STATE_CLEANOUT:
			if (status.tank3_level > settings.Seed_Level)
			{
				EnableDischargePump(true);
				SetState(&state_vars, state_vars.current_state + 1);
			}
			else
			{
				SetState(&state_vars, STATE_WAIT_TREATMENT);
			}
			break;

		//done restoring seed level and read to cycle
		case STATE_WAITREADY:
			have_error = state_vars.current_elapsed > settings.Pump3_Max_Time;
			if (have_error)
			{
				SetError(errors, ERROR_CLEANOUT);
				SetState(&state_vars, errors[ERROR_CLEANOUT].exit_state);
				if (errors->exit_state != STATE_CURRENT)
				{
					EnableDischargePump(false);
					break;
				}
			}
			else
				ClearError(errors, ERROR_CLEANOUT);

			if (status.tank3_level < settings.Seed_Level)
			{
				EnableDischargePump(false);
				SetState(&state_vars, state_vars.current_state + 1);
			}
			break;

		//waits for tank 1 to reach minimum treatment level
		//if so, turn on pump to fill tank 2
		case STATE_WAIT_TREATMENT:
			if (status.tank2_level > (settings.Min_Treatment_Level+settings.Empty_Level))
			{
				EnableFillPump(true); //tank 2 to tank 3
				WritePumpStatus();
				SetState(&state_vars, state_vars.current_state+1);
			}
			break;

			//waits for tank 3 to reach seed level
			//if so, stop filling tank 3; turn on aerator
		case STATE_WAIT_SEED:
			have_error = state_vars.current_elapsed > settings.Pump1_Max_Time;
			if (have_error)
			{
				SetError(errors, ERROR_PUMP_1);
				SetState(&state_vars, errors[ERROR_PUMP_1].exit_state);
				break;
			}
			else
				ClearError(errors, ERROR_PUMP_1);

			have_error = state_vars.current_elapsed > settings.Pump1_Slow_Time;
			if (have_error)
				{
					SetError(errors, ERROR_LOW_FLOW_1);
					SetState(&state_vars, errors[ERROR_LOW_FLOW_1].exit_state);
					if (errors->exit_state != STATE_CURRENT)
					{
						if (errors->exit_state == STATE_NEXT)
						{
							EnableFillPump(false);
							WritePumpStatus();
							EnableAerator(true);
							//SendAeratorStatus();
							WritePumpStatus();
						}
						break;
					}
				}
				else
					ClearError(errors, ERROR_LOW_FLOW_1);


			if ((status.tank3_level > (settings.Min_Treatment_Level + settings.Seed_Level))|| //normal case where there is a seed
				(status.tank2_level< settings.Empty_Level)) //startup case where there is no seed from previous cycle
			{
				EnableFillPump(false);
				WritePumpStatus();
				EnableAerator(true);
				//SendAeratorStatus();
				WritePumpStatus();
				state_vars.aerator_count = 0;
				SetState(&state_vars, state_vars.current_state+1);
				ClearError(errors, ERROR_PUMP_1);
				ClearError(errors, ERROR_LOW_FLOW_1);
			}

			break;

			//the next to states walk thru the aeration table
			//this state counts down the aerator on time; then complete, turns aerator off
		case STATE_AERATE_ON:
			if (state_vars.current_elapsed >= settings.Aeriation_Duration[2 * state_vars.aerator_count])
			{
				EnableAerator(false);
				//		SendAeratorStatus();
				SetState(&state_vars, state_vars.current_state + 1);
			}

			break;

			//this state counts down the aerator off time; then complete, turns aerator on unless at end of table, 
		case STATE_AERATE_OFF:
			if (state_vars.current_elapsed >= settings.Aeriation_Duration[2 * state_vars.aerator_count + 1])
			{
				if (state_vars.aerator_count >= settings.Aerator_Count)
				{
					EnableAerator(false);
					//SendAeratorStatus();
					WritePumpStatus();
					SetState(&state_vars, state_vars.current_state + 1);
				}
				else
				{
					EnableAerator(true);
					//SendAeratorStatus();
					WritePumpStatus();
					state_vars.aerator_count++;
					SetState(&state_vars, state_vars.current_state - 1);
					SendMsg("Remaining Aerator Cycles: ", (unsigned long)(settings.Aerator_Count- state_vars.aerator_count));
				}
			}
			break;

			//wait the required settling time; when timer is expired. pump a preset volume back into tank 1
		case STATE_SETTLE:
			if (state_vars.current_elapsed >= settings.Settling_time)
			{
				status.recycle_level = (status.tank3_level * (float)settings.Recycle_Pcnt / 100.0);
				EnableReturnPump(true);  //tank 3 to tank 1
				WritePumpStatus();
				SetState(&state_vars, state_vars.current_state+1);
			}
			break;

			//monitor tank 3 level,  when adequately reduced, stop return pump, enable discharge pump
		case STATE_RECHARGE:
			have_error = state_vars.current_elapsed > settings.Pump2_Max_Time;
			if (have_error)
			{
				SetError(errors, ERROR_PUMP_2);
				SetState(&state_vars, errors[ERROR_PUMP_2].exit_state);
				break;
			}
			else
				ClearError(errors, ERROR_PUMP_2);


			have_error = state_vars.current_elapsed > settings.Pump2_Slow_Time;

			if (have_error)
			{
				SetError(errors, ERROR_LOW_FLOW_2);
				SetState(&state_vars, errors[ERROR_LOW_FLOW_2].exit_state);
				if (errors->exit_state != STATE_CURRENT)
				{
					if (errors->exit_state == STATE_NEXT)
					{
						EnableReturnPump(false);
						WritePumpStatus();
						EnableDischargePump(true); //tank 3 to exit
					}
					break;
				}
			}
			else
				ClearError(errors, ERROR_LOW_FLOW_2);

			if (status.tank3_level < status.recycle_level)
			{
				EnableReturnPump(false);
				WritePumpStatus();
				EnableDischargePump(true); //tank 3 to exit
				SetState(&state_vars, state_vars.current_state+1);
				ClearError(errors, ERROR_PUMP_2);
				ClearError(errors, ERROR_LOW_FLOW_2);
			}
			break;

			//monitor tank 3 level; when empty, turn off pump
		case STATE_DRAIN:
			//check for failed pump
			have_error = state_vars.current_elapsed > settings.Pump3_Max_Time;
			if (have_error)
			{
				SetError(errors, ERROR_PUMP_3);
				SetState(&state_vars, errors[ERROR_PUMP_3].exit_state);
				if (errors->exit_state != STATE_CURRENT)
				{
					EnableDischargePump(false);
					WritePumpStatus();
					break;
				}
			}
			else
				ClearError(errors, ERROR_PUMP_3);

			//check for low flow
			have_error = state_vars.current_elapsed > settings.Pump3_Slow_Time;
			if (have_error)
			{
				SetError(errors, ERROR_LOW_FLOW_3);
				SetState(&state_vars, errors[ERROR_LOW_FLOW_3].exit_state);
				if (errors->exit_state != STATE_CURRENT)
				{
					EnableDischargePump(false);
					WritePumpStatus();
					break;
				}
			}
			else
				ClearError(errors, ERROR_LOW_FLOW_3);


			if (status.tank3_level < settings.Seed_Level) //need to calculate actual from percent
			{
				EnableDischargePump(false);
				WritePumpStatus();
				SetState(&state_vars, state_vars.current_state+1);
				ClearError(errors, ERROR_PUMP_3);
				ClearError(errors, ERROR_LOW_FLOW_3);
			}
			break;

			//wait idle for a while
		case STATE_IDLE:
			if (state_vars.current_elapsed >= settings.Idle_Time)
			{
				SetState(&state_vars, STATE_END);
			}
			break;

			//this state does nothing other than being a jump point to exit idle.
			//jumps immediately to start
		case STATE_END:
			SetState(&state_vars, STATE_START);
			break;

		case STATE_ABORT:
			//Someplace to park in case of a fatal error. Only way out is reboot
			//disables actuators since we got here from an error
			DisableAllActuators();
			break;
		}

		state_vars.current_elapsed++;
		state_vars.minute++;
		if ((state_vars.minute % 60) == 0)
			state_vars.hour++;
		if ((state_vars.hour % 24) == 0)
			state_vars.day++;
		if ((state_vars.day % 7) == 0)
			state_vars.week++;

		have_error = state_vars.current_elapsed > 1440; //inactive error
		if (have_error)
		{
			SetError(errors, ERROR_INACTIVE);
			SetState(&state_vars, errors[ERROR_INACTIVE].exit_state);
		}
		else
			ClearError(errors, ERROR_INACTIVE);

#ifndef UNSUPPORTED_ERROR

		have_error = state_vars.day > 7; //inactive error
		if (have_error)
		{
			SetError(errors, ERROR_OFFLINE);
			ClearError(errors, ERROR_NO_RESPONSE);
			SetState(&state_vars, errors[ERROR_OFFLINE].exit_state);
		}
		else
			ClearError(errors, ERROR_OFFLINE);

		if (!have_error)
		{
			have_error = state_vars.day > 1; //inactive error
			if (have_error)
			{
				SetError(errors, ERROR_NO_RESPONSE);
				SetState(&state_vars, errors[ERROR_NO_RESPONSE].exit_state);
			}
			else
				ClearError(errors, ERROR_NO_RESPONSE);
		}

#endif
				
		if (state_vars.status_period > settings.Reporting_Time)
		{
			state_vars.status_period = 0;
			WriteHourlyStatus();
		}

		Print_State(&state_vars);

		cout << StateHelp(&status, &settings, &state_vars )<<"\n";

		WriteLoopStatus(&status, state_vars.current_state);
	}
	return 0;
}

//sets current state and stores elapsed time
void SetState( STATE_VARS *pstate_vars, int state)
{
	if (state == STATE_CURRENT)
		return;
	pstate_vars->last_state = pstate_vars->current_state;
	pstate_vars->last_elapsed = pstate_vars->current_elapsed;

	if (state == STATE_NEXT)
		pstate_vars->current_state++;
	else
		pstate_vars->current_state=state;
	pstate_vars->current_elapsed = 0;
	WriteStateStatus(state);
}


//early diagnostic, just blinked all leds 
//( all outputs were leds rather than hardware)
void PowerUpLightCycle(void)
{
	digitalWrite(STATUS_LED_PIN, HIGH);  // On
	delay(POWERUP_BLINKRATE);
	digitalWrite(PUMP1_PIN, HIGH);  // On
	delay(POWERUP_BLINKRATE); // ms
	digitalWrite(PUMP2_PIN, HIGH);  // On
	delay(POWERUP_BLINKRATE); // ms
	digitalWrite(PUMP3_PIN, HIGH);  // On
	delay(POWERUP_BLINKRATE); // ms
	digitalWrite(AERATOR_PIN, HIGH);  // On	
	delay(POWERUP_BLINKRATE); // ms
	digitalWrite(PUMP1_PIN, LOW);  // On
	delay(POWERUP_BLINKRATE); // ms
	digitalWrite(PUMP2_PIN, LOW);  // On
	delay(POWERUP_BLINKRATE); // ms
	digitalWrite(PUMP3_PIN, LOW);  // On
	delay(POWERUP_BLINKRATE); // ms
	digitalWrite(AERATOR_PIN, LOW);  // On	
	delay(POWERUP_BLINKRATE); // ms
	digitalWrite(STATUS_LED_PIN, LOW);	  // Off
}

//displays the exit requirement to move to the next state in the state machine
char* StateHelp(STATUS* pstatus, SETTINGS* psettings, STATE_VARS* pstate_vars)
{


	switch (pstate_vars->current_state)
	{
	default:
		sprintf(pstatus->exit_requirement, "Invalid state");
		break;
	case STATE_START:
		sprintf(pstatus->exit_requirement, "Jumps automatically");
		break;
	case STATE_CLEANOUT:
		sprintf(pstatus->exit_requirement, "Jumps automatically based on whether Level 3 > or < %ld", (unsigned long)psettings->Seed_Level);
		break;
	case STATE_WAITREADY:
		sprintf(pstatus->exit_requirement, "Jumps if Level 3 < %ld", (unsigned long)psettings->Seed_Level);
		break;
	case STATE_WAIT_TREATMENT:
		sprintf(pstatus->exit_requirement, "Jumps if Level 2 > %ld", (unsigned long)psettings->Min_Treatment_Level+ (unsigned long)psettings->Empty_Level);
		break;
	case STATE_WAIT_SEED:
		sprintf(pstatus->exit_requirement, "Jumps if Level 3 > %ld", (unsigned long)psettings->Min_Treatment_Level + (unsigned long)psettings->Seed_Level);
		break;
	case STATE_AERATE_ON:
		sprintf(pstatus->exit_requirement, "Time delay of %ld", (unsigned long)(psettings->Aeriation_Duration[2 * pstate_vars->aerator_count] - pstate_vars->current_elapsed));
		break;
	case STATE_AERATE_OFF:
		sprintf(pstatus->exit_requirement, "Time delay of %ld", (unsigned long)(psettings->Aeriation_Duration[2 * pstate_vars->aerator_count + 1] - pstate_vars->current_elapsed));
		break;
	case STATE_SETTLE:
		sprintf(pstatus->exit_requirement, "Time delay of %ld", (unsigned long)(psettings->Settling_time - pstate_vars->current_elapsed));
		break;
	case STATE_RECHARGE:
		sprintf(pstatus->exit_requirement, "Jumps if Level 3 < %ld", (unsigned long)pstatus->recycle_level);
		break;
	case STATE_DRAIN:
		sprintf(pstatus->exit_requirement, "Jumps if Level 3 < %ld", (unsigned long)psettings->Seed_Level);
		break;
	case STATE_IDLE:
		sprintf(pstatus->exit_requirement, "Time delay of %ld", (unsigned long)(psettings->Idle_Time - pstate_vars->current_elapsed));
		break;
	case STATE_END:
		sprintf(pstatus->exit_requirement, "Jumps automatically");
		break;
	case STATE_ABORT:
		sprintf(pstatus->exit_requirement, "FATAL ERROR - must repair and reboot");
		break;
	}

	return pstatus->exit_requirement;
}

