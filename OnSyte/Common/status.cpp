/******************************************************************************
* File Name   : status.cpp
* Description : files common between master and slave modules of Onsyte septic control
*			  : status messaging
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

#include "../Common/status.h"
#include "../Common/error.h"
//#include "main.h"
#ifdef SLAVE_CODE
#include "../OnSyteSlave/slaveHardware.h"
#endif
#ifdef MASTER_CODE
#include "../OnSyteMaster/masterHardware.h"
#endif
#include "wiringPi.h"
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <time.h> 

using namespace std;

/*
#include <iostream>
#include <fstream>

int main()
{
	// backup cout buffer and redirect to out.txt 
std::ofstream out("out.txt");

auto* coutbuf = std::cout.rdbuf();
std::cout.rdbuf(out.rdbuf());

std::cout << "This will be redirected to file out.txt" << std::endl;

// reset cout buffer 
std::cout.rdbuf(coutbuf);

std::cout << "This will be printed on console" << std::endl;

return 0;
}
*/

//reset status values
void StatusDefaults(STATUS* pstatus)
{
	pstatus->tank1_pressure = 0;
	pstatus->tank2_pressure = 0;
	pstatus->tank3_pressure = 0;
	pstatus->tank1_level = 0;
	pstatus->tank2_level = 0;
	pstatus->tank3_level = 0;
	pstatus->pcb_temp = 0;
	pstatus->ph = 7;
	pstatus->oxygen = 0;
	memset(pstatus->exit_requirement,0, sizeof(pstatus->exit_requirement));
}

#define BLINK_PERIOD	1  //on/off/on time
#define NOBLINK_PERIOD 15 // delay between codes

//blinks current state on the status led
void BlinkStatusLED(int state)
{
	static int current_state = 0;
	static bool blink_state = false;
	static bool noblink_state = true;
	static int elapsed = NOBLINK_PERIOD;
	static int blink_count = 0;

	//if in no blink state, just count down
	if (noblink_state)
	{
		current_state = state;
		if (elapsed > NOBLINK_PERIOD) //time out, so fall thru to status blink
		{
			noblink_state = false;
			elapsed = 0;
		}
		else
		{
			elapsed++;
		}
		return;
	}

	if (blink_state == false)
	{
		if (elapsed > BLINK_PERIOD) //time out, so fall thru to status blink
		{
			blink_state = true;
			EnableStatusLED(true);
			elapsed = 0;
		}
		else
		{
			elapsed++;

		}
	}
	else
	{
		if (elapsed > BLINK_PERIOD) //time out, so fall thru to status blink
		{
			blink_state = false;
			EnableStatusLED(false);
			elapsed = 0;
			blink_count++;
			if (blink_count >= current_state)
			{
				noblink_state = true;
				blink_count = 0;
			}

		}
		else
		{
			elapsed++;
		}
	}

}


//status message stub, string only
void SendMsg(const char* msg)
{
	cout << msg << endl;
}

//status message stub, string and string tag
void SendMsg(const char* msg, char* tag)
{
	cout << msg << tag << endl;
}

//status message stub, string and long int
void SendMsg(const char* msg, unsigned long data1)
{
	cout << msg << " " << std::dec << data1 << " " << "0x" << std::hex << data1 << std::dec << endl;
}

//status message stub, float
void SendMsg(const char* msg, float data1)
{
	cout << msg << " " << data1 << endl;
}

//diagnostic, just waits for keybaord input
void MyWait(char* msg)
{
	return;
	char c;
	cout << "Wait ... " << msg << " " << endl;
	c = std::cin.get();
}

//writes the loop status file
bool WriteLoopStatus(STATUS* pstatus, int state)
{
	SendMsg("SendLoopStatus\n");

	FILE* outfile;
	outfile = fopen(LOOP_STATUS_FILENAME, "w");
	if (outfile == NULL)
		return false;

	fprintf(outfile, CurrentTime());
	fprintf(outfile, "tank2_pressure %f \n", pstatus->tank2_pressure);
	fprintf(outfile, "tank3_pressure %f \n", pstatus->tank3_pressure);
	fprintf(outfile, "tank2_level    %f \n", pstatus->tank2_level);
	fprintf(outfile, "tank3_level    %f \n", pstatus->tank3_level);	
	fprintf(outfile, "oxygen         %f \n", pstatus->oxygen);
	fprintf(outfile, "ph             %f \n", pstatus->ph);
	fprintf(outfile, "pcb_temp       %f \n", pstatus->pcb_temp);
	fprintf(outfile, "tank_ temp      %f \n", pstatus->tank_temp);
	fprintf(outfile, "%d %s\n "			, state, StateDecode(state));
	fprintf(outfile, "%s\n "			, pstatus->exit_requirement);
	fclose(outfile);

	return true;
}

//writes the state status file
bool WriteStateStatus(int state)
{
	SendMsg("SendStateStatus\n");

	FILE* outfile;
	outfile = fopen(STATE_STATUS_FILENAME, "a");
	if (outfile == NULL)
		return false;

	fprintf(outfile, CurrentTime());
	fprintf(outfile, " State %d %s\n ", state, StateDecode(state));
	fclose(outfile);

	return true;
}
// write the hourly status file
bool WriteHourlyStatus(void)
{
	FILE* outfile;
	SendMsg("SendHourlyStatus\n");
	return true;
}
// writes the pump status file ( includes the aerator
bool WritePumpStatus(void)
{
	FILE* outfile;
	SendMsg("SendPumpStatus\n");

	outfile = fopen(PUMP_STATUS_FILENAME, "a");
	if (outfile == NULL)
		return false;
	fprintf(outfile, CurrentTime());
	fprintf(outfile, " P1 = %d, ", ReadPump1());
	fprintf(outfile, " P2 = %d, ", ReadPump2());
	fprintf(outfile, " P3 = %d, ", ReadPump3());
	fprintf(outfile, " Air = %d\n", ReadAerator());
	fclose(outfile);

	return true;
}
// intended to  write the aerator status file, but now embedded in WritePumpStatus
bool WriteAeratorStatus(void)
{
	FILE* outfile;
	SendMsg("SendAeratorStatus\n");
	return true;
}
//sends overtemp to cloud if necessary
bool WriteErrorStatus(char* tag, int code)
{
	FILE* outfile;
	SendMsg("SendErrorStatus ", ErrorText(code));

	outfile = fopen(ERROR_STATUS_FILENAME, "a");
	if (outfile == NULL)
		return false;
	fprintf(outfile, CurrentTime());
	fprintf(outfile, " Error %s (%d), %s\n",tag, code, ErrorText(code));
	fclose(outfile);

	return true;
}

//reads the loop status file
bool ReadLoopStatus(STATUS* pstatus, int state)
{
	char instr[128];

	FILE* infile;
	infile = fopen(LOOP_STATUS_FILENAME, "r");
	while (!feof(infile))
	{
		fgets(instr, 127, infile);
	}

	fclose(infile);
}

//reads the state status file
bool ReadStateStatus(int state)
{

}

//reads the hourly status file
bool ReadHourlyStatus(void)
{

}
//reads the pump status file ( currently includes aerator
bool ReadPumpStatus(void)
{

}
// intended to read aerator status file, but currently that function is embedded in ReadPumpStatus
bool ReadAeratorStatus(void)
{

}

//reads the error status file
bool ReadErrorStatus(void)
{

}

//returns a standard string representation of the current time
char* CurrentTime(void)
{
	//static char timestr[80];

	time_t rawtime;
	struct tm* timeinfo;

	time(&rawtime);
	timeinfo = localtime(&rawtime);
	//sprintf(timestr, "The current date/time is: %s \n", asctime(timeinfo));
	return asctime(timeinfo);
}
//parses the standard strin time as written by CurrentTime
struct tm ParseTime(char* instr)
{
	bool status;
	struct tm timeinfo;

	status = strptime(instr, "%a %b %d %H:%M:%S %Y", &timeinfo);

	if (!status)
	{
		timeinfo.tm_mon = 0;
		timeinfo.tm_mday = 1;
		timeinfo.tm_year = 0;
		timeinfo.tm_hour = 0;
		timeinfo.tm_min = 0; 
		timeinfo.tm_sec = 0;
	}

	return timeinfo;

}

