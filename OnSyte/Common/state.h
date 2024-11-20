#pragma once

#include "../Common/status.h"

struct STATE_VARS
{
	int current_state;
	int last_state;
	int current_elapsed;
	int last_elapsed;
	int aerator_count;
	int status_period;
	int minute;
	int hour;
	int day;
	int week;
};

enum STATES
{
	STATE_START,			//Start placeholder
	STATE_CLEANOUT,			//pumo if tank 3 not normal
	STATE_WAITREADY,            //waiting for normal level in 3
	STATE_WAIT_TREATMENT,	//Waiting for level in tank 2 to level requirement
	STATE_WAIT_SEED,		//Waiting for level in tank 3 to level requirement (P1 on)
	STATE_AERATE_ON,		//Waiting for Aerator on timer to expire
	STATE_AERATE_OFF,		//Waiting for Aerator off timer to expire
	STATE_SETTLE,			//Waiting for Settling timer to expire
	STATE_RECHARGE,			//Waiting for level in tank 3 to drop to level requirement (P2 on)
	STATE_DRAIN,			//Waiting for level in tank 3 to drop to level requirement (P3 on)
	STATE_IDLE,				//Waiting for Idle timer to expire
	STATE_END,				//End placeholder
	STATE_ABORT,			//Abort stops here permanently on error
	STATE_CURRENT,			//Not a coded state, but the error code returns a state which means same state
	STATE_NEXT				//Not a coded state, but the error code returns a state which means use next state
};

char* StateDecode(int state);
void SetState(STATE_VARS* pstate_vars, int state);
void Print_State(STATE_VARS* pstate_vars);
void StateDefaults(STATE_VARS* pstatevars);
