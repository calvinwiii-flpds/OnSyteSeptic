/******************************************************************************
* File Name   : error.cpp
* Description : files common between master and slave modules of Onsyte septic control
*			  : error codes and handling
* Author      : Calvin Winey
* Created On  : 11/14/2024
* Version     : see revision.h
* MCU         : <>
* Board       : Raspberry Pi
* Protocol    : 
* Compiler    : Visual Studio 19 remote compiled on pi using gcc
* Revision History:
*				see revision.h
* Notes:
*    all files common between Master and Slave extracted and placed in common folder
*
******************************************************************************/

#include <stdio.h>
#include <iostream>
#include <string.h>
#include "error.h"
#include "status.h"

//sets individual error to its initial state
void ClearError(ERROR_ENTRY *errors, int code)
{
	errors[code].active = false;
	errors[code].sent = false;
	errors[code].error_count = 0;
	switch (code)
	{
	case ERROR_TANK_TEMP:
		sprintf(errors[code].tag, "UNDF");
		errors[code].exit_state = STATE_ABORT;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_CPU_TEMP:
		sprintf(errors[code].tag, "UNDF");
		errors[code].exit_state = STATE_ABORT;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;	
	case ERROR_WASHOUT:
		sprintf(errors[code].tag, "OS9");
		errors[code].exit_state = STATE_START;
		errors[code].level = ERRORLEVEL_SEVERE;
		break;  
	case ERROR_PUMP_1:
		sprintf(errors[code].tag, "XF9");
		errors[code].exit_state = STATE_START;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_PUMP_2:
		sprintf(errors[code].tag, "DS9");
		errors[code].exit_state = STATE_START;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_PUMP_3:
		sprintf(errors[code].tag, "WS9");
		errors[code].exit_state = STATE_NEXT; //next state rather than specific state
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_LOW_FLOW_1:
		sprintf(errors[code].tag, "XF4");
		errors[code].exit_state = STATE_CURRENT;  //keep doing what your doing
		errors[code].level = ERRORLEVEL_MINOR;
		break;
	case ERROR_LOW_FLOW_2:
		sprintf(errors[code].tag, "DS4");
		errors[code].exit_state = STATE_CURRENT;	//keep doing what your doing
		errors[code].level = ERRORLEVEL_MINOR;
		break;
	case ERROR_LOW_FLOW_3:
		sprintf(errors[code].tag, "WS4");
		errors[code].exit_state = STATE_CURRENT; //keep doing what your doing
		errors[code].level = ERRORLEVEL_MINOR;
		break;
	case ERROR_LEVEL_1:
		sprintf(errors[code].tag, "UNDF");
		errors[code].exit_state = STATE_ABORT;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break; 
	case ERROR_LEVEL_2:
		sprintf(errors[code].tag, "RX9");
		errors[code].exit_state = STATE_ABORT;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_LEVEL_3:
		sprintf(errors[code].tag, "DX9");
		errors[code].exit_state = STATE_NEXT; //next state rather than specific state
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_TANK_1:
		sprintf(errors[code].tag, "UNDF");
		errors[code].exit_state = STATE_START;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_TANK_2:
		sprintf(errors[code].tag, "FL1");
		errors[code].exit_state = STATE_START;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_TANK_3:
		sprintf(errors[code].tag, "FL2");
		errors[code].exit_state = STATE_START;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;	
	case ERROR_CLEANOUT:
		sprintf(errors[code].tag, "DT1");
		errors[code].exit_state = STATE_START;
		errors[code].level = ERRORLEVEL_SERIOUS;
		break; 
	case ERROR_RATION:
		sprintf(errors[code].tag, "RA9");
		errors[code].exit_state = STATE_NEXT; //next state rather than specific state
		errors[code].level = ERRORLEVEL_SERIOUS;
		break;
	case ERROR_NO_RESPONSE:
		sprintf(errors[code].tag, "MP9");
		errors[code].exit_state = STATE_CURRENT; //keep doing what your doing
		errors[code].level = ERRORLEVEL_MINOR;
		break;
	case ERROR_OFFLINE:
		sprintf(errors[code].tag, "RA9");
		errors[code].exit_state = STATE_CURRENT; //keep doing what your doing
		errors[code].level = ERRORLEVEL_MINOR;
		break;
	case ERROR_INACTIVE:
		sprintf(errors[code].tag, "LF4");
		errors[code].exit_state = STATE_CURRENT; //keep doing what your doing
		errors[code].level = ERRORLEVEL_MINOR;
		break;
	}
}
//Initialization function
//Executes ClearError for all error states
void ClearAllErrors(ERROR_ENTRY *perrors)
{
	for (int i = 0; i < MAX_ERRORS; i++)
	{
		ClearError(perrors,i);
	}
}

//returns a description string for an error
char* ErrorText(int code)
{
	static char outstr[128];
	switch (code)
	{
	default:
		sprintf(outstr, "Error undefined");
		break;
	case ERROR_TANK_TEMP:
		sprintf(outstr, "Tank Overtemperature");
		break;
	case ERROR_CPU_TEMP:
		sprintf(outstr, "CPU Overtemperature");
		break;
	case ERROR_WASHOUT:
		sprintf(outstr, "Washout");
		break;
	case ERROR_PUMP_1:
		sprintf(outstr, "Pump 1 Timeout");
		break;
	case ERROR_PUMP_2:
		sprintf(outstr, "Pump 2 Timeout");
		break;
	case ERROR_PUMP_3:
		sprintf(outstr, "Pump 3 Timeout");
		break;
	case ERROR_LOW_FLOW_1:
		sprintf(outstr, "Pump 1 Low Flow");
		break;
	case ERROR_LOW_FLOW_2:
		sprintf(outstr, "Pump 2 Low Flow");
		break;
	case ERROR_LOW_FLOW_3:
		sprintf(outstr, "Pump 3 Low Flow");
		break;
	case ERROR_LEVEL_1:
		sprintf(outstr, "No Level 1 sense at this time");
		break;
	case ERROR_LEVEL_2:
		sprintf(outstr, "tank 2 level invalid");
		break;
	case ERROR_LEVEL_3:
		sprintf(outstr, "tank 3 level invalid");
		break;
	case ERROR_TANK_1:
		sprintf(outstr, "No Level 1 sense at this time");
		break;
	case ERROR_TANK_2:
		sprintf(outstr, "tank 2 level too high, but valid");
		break;
	case ERROR_TANK_3:
		sprintf(outstr, "tank 3 level too high, but valid");
		break;
	case ERROR_CLEANOUT:
		sprintf(outstr, "Cleanout timeout");
		break;
	case ERROR_RATION:
		sprintf(outstr, "Ration Timeout");
		break;
	case  ERROR_NO_RESPONSE:
		sprintf(outstr, "Offline 24hrs");
		break;
	case  ERROR_OFFLINE:
		sprintf(outstr, "Offline 7 days");
		break;
	case  ERROR_INACTIVE:
		sprintf(outstr, "Inactive 24hrs");
		break;
	}
	return outstr;
}


//Sets an individual error and appropriate flags
//writes the error log, but only on first occurace of the error
void SetError(ERROR_ENTRY* errors, int code)
{
	errors[code].active = true;

	errors[code].error_count ++;
	if (!errors[code].sent )
	{
		WriteErrorStatus(errors[code].tag, code);
		errors[code].sent = true;
	}
}
