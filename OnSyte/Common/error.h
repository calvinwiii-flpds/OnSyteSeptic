#pragma once

#include "status.h"
#include "state.h"
#include "settings.h"

#define ERROR_TANK_TEMP		0   //UNDF   tank temperature high
#define ERROR_CPU_TEMP		1	//UNDF   adc tenmperature high
#define ERROR_WASHOUT       2   //OS9    tank 2 and tank 3 levels too high
#define ERROR_PUMP_1		3   //XF9	 pump 1 timeout
#define ERROR_PUMP_2		4	//DS9	 pump 2 timeout
#define ERROR_PUMP_3		5	//WS9	 pump 3 timeout
#define ERROR_LOW_FLOW_1	6	//XF4	 Pump 1 low flow
#define ERROR_LOW_FLOW_2	7	//DF4	 Pump 2 low flow
#define ERROR_LOW_FLOW_3	8	//wF4	 Pump 3 low flow
#define ERROR_LEVEL_1       9   //No Level 1 sense at this time
#define ERROR_LEVEL_2		10	//RX9	tank 2 level invalid
#define ERROR_LEVEL_3		11	//DX9	tank 3 level invalid
#define ERROR_TANK_1		12	//No Level 1 sense at this time
#define ERROR_TANK_2		13	//FL1	tank 2 level too high, but valid
#define ERROR_TANK_3		14	//FL2	tank 3 level too high, but valid
#define ERROR_CLEANOUT      15  //DT1   ??
#define ERROR_RATION		16	//RA9   ??
#define ERROR_NO_RESPONSE	17	//MP9   system offline 1 daya Marco polo error
#define ERROR_OFFLINE		18	//SO9   system offline 7 days
#define ERROR_INACTIVE		19  //LF4   No activity for 24 hrs
#define MAX_ERRORS			20

#define ERRORLEVEL_SEVERE	1	//Severe alarm state. Only code to signal sounder and strobe light. Notify OnSyte immediately via SMS/Email	
#define ERRORLEVEL_SERIOUS  2	//Notify OnSyte immediately via SMS/Email, illuminate alarm icon on visual screen.	
#define ERRORLEVEL_MINOR	3	//Notify OnSyte during  handshake/status report. Do not illuminate alarm icon on screen.	

struct ERROR_ENTRY
{
	char tag[4];
	bool sent;
	bool active;
	int level;
	int error_count;
	int exit_state;
};

void ClearError(ERROR_ENTRY* errors, int code);
void ClearAllErrors(ERROR_ENTRY* errors);
char* ErrorText(int code);
void SetError(ERROR_ENTRY* errors, int code);

