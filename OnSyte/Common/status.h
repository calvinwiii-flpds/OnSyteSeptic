#pragma once

#include "state.h"
#include "settings.h"
#include "error.h"

#define LOOP_STATUS_FILENAME		"/home/cwiney/Desktop/LoopStatus.txt"
#define STATE_STATUS_FILENAME		"/home/cwiney/Desktop/StateStatus.txt"
#define HOURLY_STATUS_FILENAME		"/home/cwiney/Desktop/HourlyStatus.txt"
#define PUMP_STATUS_FILENAME		"/home/cwiney/Desktop/PumpStatus.txt"
//#define AERATOR_STATUS_FILENAME		"/home/cwiney/Desktop/AeratorStatus.txt"  //combined with pump ststus
#define ERROR_STATUS_FILENAME    "/home/cwiney/Desktop/ErrorStatus.txt"


struct STATUS
{
	float tank1_pressure;
	float tank2_pressure;
	float tank3_pressure;
	float tank1_level;
	float tank2_level;
	float tank3_level;
	float tank_temp;
	float pcb_temp;
	float ph;
	float oxygen;
	float recycle_level; //needed for percentage calculations
	char  exit_requirement[128];
};

void StatusDefaults(STATUS* pstatus);
void BlinkStatusLED(int state);
void SendMsg(const char* msg);
void SendMsg(const char* msg, char* tag);
void SendMsg(const char* msg, unsigned long data1);
void SendMsg(const char* msg, float data1);
void MyWait(char* txt);


bool WriteLoopStatus(STATUS* pstatus, int state);
bool WriteStateStatus(int state);
bool WriteHourlyStatus(void);
bool WritePumpStatus(void);
bool WriteAeratorStatus(void);
bool WriteErrorStatus(char *tag, int code);

bool ReadLoopStatus(STATUS* pstatus, int state);
bool ReadStateStatus(int state);
bool ReadHourlyStatus(void);
bool ReadPumpStatus(void);
bool ReadAeratorStatus(void);
bool ReadErrorStatus(void);



char* CurrentTime(void);
struct tm ParseTime(char* instr);

