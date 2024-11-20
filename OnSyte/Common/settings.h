#pragma once
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <sys/stat.h>

#define SETTINGS_FILENAME		"/home/cwiney/Desktop/settings.txt"

#define AERATOR_BUFSIZE      50

#define DEMO_MAIN_LOOP_TIME   2	//loop time in seconds
#define DEMO_AERATOR_TIME_ON  4 //minutes
#define DEMO_AERATOR_TIME_OFF 2 //minutes
#define DEMO_AERATOR_COUNT    3 //must be less than AERATOR_BUFFSIZE
#define DEMO_SETTLE_TIME      2 //minutes
#define DEMO_IDLE_TIME        5 //minutes
#define DEMO_REPORTING_TIME	60	//minutes


#define MAIN_LOOP_TIME		60	//loop time in seconds
#define AERATOR_TIME_ON		30  //minutes
#define AERATOR_TIME_OFF	10  //minutes
#define AERATOR_COUNT    AERATOR_BUFSIZE //must be less than AERATOR_BUFFSIZE
#define SETTLE_TIME			30  //minutes
#define IDLE_TIME			30	//minutes
#define REPORTING_TIME		60	//minutes
#define  PUMP1_SLOW_TIME	10	//minutes
#define  PUMP2_SLOW_TIME	10	//minutes
#define  PUMP3_SLOW_TIME	10	//minutes
#define  PUMP1_MAX_TIME		15	//minutes
#define  PUMP2_MAX_TIME		15	//minutes
#define  PUMP3_MAX_TIME		15	//minutes
#define  TANK2_HIGH_LIMIT	40  //inches
#define  TANK3_HIGH_LIMIT	40	//inches
#define  TANK2_SENSOR_LIMIT	60	//inches
#define  TANK3_SENSOR_LIMIT	60  //inches

#define T_SCALE			1000
#define T_OFFSET		0
#define PH_SCALE		1000
#define PH_OFFSET		0
#define DO_SCALE		1000
#define DO_OFFSET		0

#define MIN_TREATMENT_LEVEL	200	//gallons
#define SEED_LEVEL			50	//gallons
#define RECYCLE_PCNT		25	//percent 
#define FEILD_PCNT			50	//percent 
#define EMPTY_LEVEL			10	//gallons
#define CPU_OVER_TEMP       70	//degrees C
#define TANK_OVER_TEMP      60	//degrees C

#define INACTIVE_TIME       1440    //60 minutes x 24 hours

struct CALIB
{
	unsigned long Scale;							//counts per degree etc
	long		  Offset;							//counts to zero
};

struct SETTINGS
{
	unsigned int  Min_Treatment_Level;				// UnSigned Word, ADC Counts, 0 – 65, 535.
	unsigned int  Seed_Level;						// UnSigned Word, ADC Counts, 0 – 65, 535.
	unsigned int  Aeriation_Duration[2 * AERATOR_BUFSIZE];	// UnSigned Word, number of minutes, 0 – 64, 734 minutes
	unsigned int  Settling_time;					// UnSigned Word, number of minutes, 0 – 64, 734 minutes
	unsigned int  Recycle_Pcnt;						// % (Word:%)
	unsigned int  Feild_Pcnt;						// (Word:%)
	char          URL[1000];						// (text Array, > 1K)
	unsigned int  Idle_Time;						// (Word: Minutes)
	unsigned long Tank2_Level_Lookup[50];			// (50 - Word x Word / Voltage x Gallons)
	unsigned long Tank3_Level_Lookup[50];			// (50 - Word x Word / Voltage x Gallons)
	unsigned int  Reporting_Time;					// (Word: Minutes) Status reporting time
	unsigned int  Empty_Level;
	unsigned int  Main_Loop_Time;					 //normally 60 seconds, but this allows for debugging
	unsigned int  Aerator_Count;

	unsigned int  CPU_Over_Temp;                         //degrees  
	unsigned int  Tank_Over_Temp;                         //degrees  
	unsigned int  Pump1_Slow_Time;
	unsigned int  Pump2_Slow_Time;
	unsigned int  Pump3_Slow_Time;
	unsigned int  Pump1_Max_Time;
	unsigned int  Pump2_Max_Time;
	unsigned int  Pump3_Max_Time;
	unsigned int  Tank2_High_Limit;
	unsigned int  Tank3_High_Limit;
	unsigned int  Tank2_Sensor_Limit;
	unsigned int  Tank3_Sensor_Limit;
	CALIB		  T_Cal;
	CALIB		  Ph_Cal;
	CALIB		  DO_Cal;
};

bool ReadSettings(FILE* infile, SETTINGS* psettings);
bool WriteSettings(FILE* outfile, SETTINGS* psettings);
void SettingDefaults(SETTINGS* psettings);
void SettingDemoDefaultsChanges(SETTINGS* psettings);
void SettingCleared(SETTINGS* psettings);
bool FileChanged(char* filename, __time_t* file_time);
