/******************************************************************************
* File Name   : state.cpp
* Description : files common between master and slave modules of Onsyte septic control
*			  : state machine states 
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

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <thread>


#include "state.h"


using namespace std;

//displays the current state
void Print_State(STATE_VARS* pstate_vars)
{
	cout << StateDecode(pstate_vars->current_state);
	cout << " cycle ";
	cout << pstate_vars->current_elapsed;
	cout << "\n";
	
}

//returns string representation of a state number
char* StateDecode(int state)
{
	switch (state)
	{
	case STATE_START:
		return "STATE_START";
		break;
	case STATE_CLEANOUT:
		return "STATE_CLEANOUT";
		break;
	case STATE_WAITREADY:
		return "STATE_WAITREADY";
		break;
	case STATE_WAIT_TREATMENT:
		return "STATE_WAIT_TREATMENT";
		break;
	case STATE_WAIT_SEED:
		return "STATE_WAIT_SEED";
		break;
	case STATE_AERATE_ON:
		return "STATE_AERATE_ON";
		break;
	case STATE_AERATE_OFF:
		return "STATE_AERATE_OFF";
		break;
	case STATE_SETTLE:
		return "STATE_SETTLE";
		break;
	case STATE_RECHARGE:
		return "STATE_RECHARGE";
		break;
	case STATE_DRAIN:
		return "STATE_DRAIN";
		break;
	case STATE_IDLE:
		return "STATE_IDLE";
		break;
	case STATE_END:
		return "STATE_END";
		break;
	case STATE_ABORT:
		return "STATE_ABORT";
		break;
	}

}


//setup state machine variables
void StateDefaults(STATE_VARS* pstatevars)
{
	pstatevars->current_state = STATE_START;
	pstatevars->last_state = STATE_START;
	pstatevars->current_elapsed = 0;
	pstatevars->last_elapsed = 0;
	pstatevars->aerator_count = 0;
	pstatevars->status_period = 0;
	pstatevars->minute = 0;
	pstatevars->hour = 0;
	pstatevars->day = 0;
	pstatevars->week = 0;


}