/******************************************************************************
* File Name   : settings.cpp
* Description : files common between master and slave modules of Onsyte septic control
*			  : user settings
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

#include "settings.h"

//default settings, should be overwritten by a cloud function
void SettingDefaults(SETTINGS* psettings)
{
	int i;

	psettings->Min_Treatment_Level	= MIN_TREATMENT_LEVEL;		// UnSigned Word, ADC Counts, 0 – 65, 535.
	psettings->Seed_Level			= SEED_LEVEL;				// UnSigned Word, ADC Counts, 0 – 65, 535.
	for (i = 0; i < 2*AERATOR_BUFSIZE; i += 2)
	{
		psettings->Aeriation_Duration[i]	= AERATOR_TIME_ON;		// UnSigned Word, number of minutes, 0 – 64, 734 minutes
		psettings->Aeriation_Duration[i + 1] = AERATOR_TIME_OFF;	// UnSigned Word, number of minutes, 0 – 64, 734 minutes
	}

	psettings->Settling_time	= SETTLE_TIME;			// UnSigned Word, number of minutes, 0 – 64, 734 minutes
	psettings->Recycle_Pcnt		= RECYCLE_PCNT;			// Unsigned Byte, precentage, 0 - 100 %, 101 % -255 % invalid
	psettings->Feild_Pcnt		= FEILD_PCNT;			// (Word:%)
	//psettings->URL="http:\\test.com";				// (text Array, > 1K)
	psettings->Idle_Time		= IDLE_TIME;				// (Word: Minutes)

//825 DZ Concrete tank
	for (unsigned long i = 0; i < 50; i++)
	{
		//	psettings->Tank2_Level_Lookup[i] = 335544 * i; // (50 - Word x Word / Voltage x Gallons)
		psettings->Tank2_Level_Lookup[i] = (int)(8.68 * (float)i); // (50 - Word x Word / gallons x inH20)
		if (i < 16)
			psettings->Tank3_Level_Lookup[i] = (int)(4.33 * (float)i); // (50 - Word x Word / gallons x inH20)
		else
			psettings->Tank3_Level_Lookup[i] = (int)(10.285 * (float)i - 85.51); // (50 - Word x Word / gallons x inH20)

		//cout << i << " : " << psettings->Tank_Level_Lookup[i] << "\n";
	}

	psettings->Reporting_Time	= REPORTING_TIME;			//(Word: Minutes) Status reporting time
	psettings->Empty_Level		= EMPTY_LEVEL;                  //technically, should be zero but may need tolerance
	psettings->Main_Loop_Time	= MAIN_LOOP_TIME;					 //normally 60 seconds, but this allows for debugging
	psettings->CPU_Over_Temp	= CPU_OVER_TEMP;
	psettings->Tank_Over_Temp	= TANK_OVER_TEMP;
	psettings->Aerator_Count	= AERATOR_COUNT;					 //normally 60 seconds, but this allows for debugging
	psettings->T_Cal.Scale		= T_SCALE;
	psettings->T_Cal.Offset		= T_OFFSET;
	psettings->Ph_Cal.Scale		= PH_SCALE;
	psettings->Ph_Cal.Offset	= PH_OFFSET;
	psettings->DO_Cal.Scale		= DO_SCALE;
	psettings->DO_Cal.Offset	= DO_OFFSET;

	psettings->CPU_Over_Temp	= CPU_OVER_TEMP;                         //degrees  
	psettings->Tank_Over_Temp	= TANK_OVER_TEMP;                         //degrees  
	psettings->Pump1_Slow_Time	= PUMP1_SLOW_TIME;
	psettings->Pump2_Slow_Time	= PUMP2_SLOW_TIME;
	psettings->Pump3_Slow_Time	= PUMP3_SLOW_TIME;
	psettings->Pump1_Max_Time	= PUMP1_MAX_TIME;
	psettings->Pump2_Max_Time	= PUMP2_MAX_TIME;
	psettings->Pump3_Max_Time	= PUMP3_MAX_TIME;
	psettings->Tank2_High_Limit = TANK2_HIGH_LIMIT;
	psettings->Tank3_High_Limit = TANK3_HIGH_LIMIT;
	psettings->Tank2_Sensor_Limit = TANK2_SENSOR_LIMIT;
	psettings->Tank3_Sensor_Limit = TANK3_SENSOR_LIMIT;
}

//demo version of the above whos purpose is to run the system quickly for the purpose of in lab debugging
//Note: only contains changes. SettingsDefaults must be called first
void SettingDemoDefaultsChanges(SETTINGS* psettings)
{
	int i;

	//psettings->Min_Treatment_Level = MIN_TREATMENT_LEVEL;		// UnSigned Word, ADC Counts, 0 – 65, 535.
	//psettings->Seed_Level = SEED_LEVEL;				// UnSigned Word, ADC Counts, 0 – 65, 535.
	for (i = 0; i < 2*AERATOR_BUFSIZE; i += 2)
	{
		psettings->Aeriation_Duration[i] = DEMO_AERATOR_TIME_ON;		// UnSigned Word, number of minutes, 0 – 64, 734 minutes
		psettings->Aeriation_Duration[i + 1] = DEMO_AERATOR_TIME_OFF;		// UnSigned Word, number of minutes, 0 – 64, 734 minutes
	}

	psettings->Settling_time = DEMO_SETTLE_TIME;			// UnSigned Word, number of minutes, 0 – 64, 734 minutes
	//psettings->Recycle_Pcnt = RECYCLE_PCNT;			// Unsigned Byte, precentage, 0 - 100 %, 101 % -255 % invalid
	//psettings->Feild_Pcnt = FEILD_PCNT;				// (Word:%)
	//psettings->URL="http:\\test.com";				// (text Array, > 1K)
	psettings->Idle_Time = DEMO_IDLE_TIME;				// (Word: Minutes)

	//825 DZ Concrete tank
	/*for (unsigned long i = 0; i < 50; i++)
	{
	//	psettings->Tank2_Level_Lookup[i] = 335544 * i; // (50 - Word x Word / Voltage x Gallons)
		psettings->Tank2_Level_Lookup[i] = (int)(8.68 * (float)i); // (50 - Word x Word / gallons x inH20)
		if(i<16)
			psettings->Tank3_Level_Lookup[i] = (int)(4.33 * (float)i); // (50 - Word x Word / gallons x inH20)
		else
			psettings->Tank3_Level_Lookup[i] = (int)(10.285 * (float)i-85.51); // (50 - Word x Word / gallons x inH20)

		//cout << i << " : " << psettings->Tank_Level_Lookup[i] << "\n";
	}*/

	psettings->Reporting_Time = DEMO_REPORTING_TIME;			//(Word: Minutes) Status reporting time
	//psettings->Empty_Level = EMPTY_LEVEL;                  //technically, should be zero but may need tolerance
	psettings->Main_Loop_Time = DEMO_MAIN_LOOP_TIME;					 //normally 60 seconds, but this allows for debugging
	//psettings->CPU_Over_Temp = CPU_OVER_TEMP;
	//psettings->Tank_Over_Temp = TANK_OVER_TEMP;
	psettings->Aerator_Count = DEMO_AERATOR_COUNT;					 //normally 60 seconds, but this allows for debugging
	//psettings->T_Cal.Scale = T_SCALE;
	//psettings->T_Cal.Offset = T_OFFSET;
	//psettings->Ph_Cal.Scale = PH_SCALE;
	//psettings->Ph_Cal.Offset = PH_OFFSET;
	//psettings->DO_Cal.Scale = DO_SCALE;
	///psettings->DO_Cal.Offset = DO_OFFSET;

	//psettings->CPU_Over_Temp = CPU_OVER_TEMP;                         //degrees  
	//psettings->Tank_Over_Temp = TANK_OVER_TEMP;                         //degrees  
	//psettings->Pump1_Slow_Time = PUMP1_SLOW_TIME;
	//psettings->Pump2_Slow_Time = PUMP2_SLOW_TIME;
	//psettings->Pump3_Slow_Time = PUMP3_SLOW_TIME;
	//psettings->Pump1_Max_Time = PUMP1_MAX_TIME;
	//psettings->Pump2_Max_Time = PUMP2_MAX_TIME;
	//psettings->Pump3_Max_Time = PUMP3_MAX_TIME;
	//psettings->Tank2_High_Limit = TANK2_HIGH_LIMIT;
	//psettings->Tank3_High_Limit = TANK3_HIGH_LIMIT;
	//psettings->Tank2_Sensor_Limit = TANK2_SENSOR_LIMIT;
	//psettings->Tank3_Sensor_Limit = TANK3_SENSOR_LIMIT;
}

//cleared settings, for debugging read/writes, does not generate functional code
void SettingCleared(SETTINGS* psettings)
{
	int i;

	psettings->Min_Treatment_Level = 0;		// UnSigned Word, ADC Counts, 0 – 65, 535.
	psettings->Seed_Level = 0;				// UnSigned Word, ADC Counts, 0 – 65, 535.
	for (i = 0; i < 2*AERATOR_BUFSIZE; i += 2)
	{
		psettings->Aeriation_Duration[i] = 0;		// UnSigned Word, number of minutes, 0 – 64, 734 minutes
		psettings->Aeriation_Duration[i + 1] = 0;		// UnSigned Word, number of minutes, 0 – 64, 734 minutes
	}

	psettings->Settling_time = 0;			// UnSigned Word, number of minutes, 0 – 64, 734 minutes
	psettings->Recycle_Pcnt = 0;			// Unsigned Byte, precentage, 0 - 100 %, 101 % -255 % invalid
	psettings->Feild_Pcnt = 0;				// (Word:%)
	//psettings->URL="http:\\test.com";				// (text Array, > 1K)
	psettings->Idle_Time = 0;				// (Word: Minutes)
	for (unsigned long i = 0; i < 50; i++)
	{
		psettings->Tank2_Level_Lookup[i] = 0; // (50 - Word x Word / Voltage x Gallons)
		psettings->Tank3_Level_Lookup[i] = 0; // (50 - Word x Word / Voltage x Gallons)

		//cout << i << " : " << psettings->Tank_Level_Lookup[i] << "\n";
	}

	psettings->Reporting_Time = 0;			//(Word: Minutes) Status reporting time
	psettings->Empty_Level = 0;                  //technically, should be zero but may need tolerance
	psettings->Main_Loop_Time = 0;
	psettings->CPU_Over_Temp = 0;
	psettings->Tank_Over_Temp = 0;
	psettings->Aerator_Count = 0;
	psettings->T_Cal.Scale = 0;
	psettings->T_Cal.Offset = 0;
	psettings->Ph_Cal.Scale = 0;
	psettings->Ph_Cal.Offset = 0;
	psettings->DO_Cal.Scale = 0;
	psettings->DO_Cal.Offset = 0;

	psettings->CPU_Over_Temp = 0;                          
	psettings->Tank_Over_Temp = 0;                          
	psettings->Pump1_Slow_Time = 0;
	psettings->Pump2_Slow_Time = 0;
	psettings->Pump3_Slow_Time = 0;
	psettings->Pump1_Max_Time = 0;
	psettings->Pump2_Max_Time = 0;
	psettings->Pump3_Max_Time = 0;
	psettings->Tank2_High_Limit = 0;
	psettings->Tank3_High_Limit = 0;
	psettings->Tank2_Sensor_Limit = 0;
	psettings->Tank3_Sensor_Limit = 0;
}

//reads settings from a file
bool ReadSettings(FILE* infile, SETTINGS* psettings)
{
	bool status;
	status = true;
	char instr[128];
	char* datapos;
	int index;

	while (!feof(infile))
	{
		fgets(instr, 127, infile);
		datapos = strchr(instr, ' ');

		if (datapos != NULL)
		{
			if (strstr(instr, "Min_Treatment_Level"))
				sscanf(datapos, "%d", &(psettings->Min_Treatment_Level));
			if (strstr(instr, "Seed_Level"))
				sscanf(datapos, "%d", &(psettings->Seed_Level));

			if (strstr(instr, "Aeriation_Duration"))
			{
				if (isdigit((datapos - 2)[0]))
					sscanf(datapos - 2, "%d", &index);
				else
					sscanf(datapos - 1, "%d", &index);

				sscanf(datapos, "%d", &(psettings->Aeriation_Duration[index]));
			}

			if (strstr(instr, "Settling_time"))
				sscanf(datapos, "%d", &(psettings->Settling_time));
			if (strstr(instr, "Recycle_Pcnt"))
				sscanf(datapos, "%d", &(psettings->Recycle_Pcnt));
			if (strstr(instr, "Feild_Pcnt"))
				sscanf(datapos, "%d", &(psettings->Feild_Pcnt));
			if (strstr(instr, "Idle_Time"))
				sscanf(datapos, "%d", &(psettings->Idle_Time));

			if (strstr(instr, "Tank2_Level_Lookup"))
			{
				if (isdigit((datapos - 2)[0]))
					sscanf(datapos - 2, "%d", &index);
				else
					sscanf(datapos - 1, "%d", &index);
				sscanf(datapos, "%d", &(psettings->Tank2_Level_Lookup[index]));
			}

			if (strstr(instr, "Tank3_Level_Lookup"))
			{
				if (isdigit((datapos - 2)[0]))
					sscanf(datapos - 2, "%d", &index);
				else
					sscanf(datapos - 1, "%d", &index);
				sscanf(datapos, "%d", &(psettings->Tank3_Level_Lookup[index]));
			}

			if (strstr(instr, "Reporting_Time"))
				sscanf(datapos, "%d", &(psettings->Reporting_Time));

			if (strstr(instr, "Empty_Level"))
				sscanf(datapos, "%d", &(psettings->Empty_Level));

			if (strstr(instr, "CPU_Over_Temp"))
				sscanf(datapos, "%d", &(psettings->CPU_Over_Temp));

			if (strstr(instr, "Tank_Over_Temp"))
				sscanf(datapos, "%d", &(psettings->Tank_Over_Temp));

			if (strstr(instr, "Main_Loop_Time"))
				sscanf(datapos, "%d", &(psettings->Main_Loop_Time));

			if (strstr(instr, "Aerator_Count"))
				sscanf(datapos, "%d", &(psettings->Aerator_Count));

			if (strstr(instr, "T_Scale"))
				sscanf(datapos, "%d", &(psettings->T_Cal.Scale));

			if (strstr(instr, "T_Offset"))
				sscanf(datapos, "%d", &(psettings->T_Cal.Offset));

			if (strstr(instr, "Ph_Scale"))
				sscanf(datapos, "%d", &(psettings->Ph_Cal.Scale));

			if (strstr(instr, "PH_Offset"))
				sscanf(datapos, "%d", &(psettings->Ph_Cal.Offset));

			if (strstr(instr, "DO_Scale"))
				sscanf(datapos, "%d", &(psettings->DO_Cal.Scale));

			if (strstr(instr, "DO_Offset"))
				sscanf(datapos, "%d", &(psettings->DO_Cal.Offset));

			if (strstr(instr, "Pump1_Slow_Time"))
				sscanf(datapos, "%d", &(psettings->Pump1_Slow_Time));

			if (strstr(instr, "Pump2_Slow_Time"))
				sscanf(datapos, "%d", &(psettings->Pump2_Slow_Time));

			if (strstr(instr, "Pump3_Slow_Time"))
				sscanf(datapos, "%d", &(psettings->Pump3_Slow_Time));

			if (strstr(instr, "Pump1_Max_Time"))
				sscanf(datapos, "%d", &(psettings->Pump1_Max_Time));

			if (strstr(instr, "Pump2_Max_Time"))
				sscanf(datapos, "%d", &(psettings->Pump2_Max_Time));

			if (strstr(instr, "Pump3_Max_Time"))
				sscanf(datapos, "%d", &(psettings->Pump3_Max_Time));

			if (strstr(instr, "Tank2_High_Limit"))
				sscanf(datapos, "%d", &(psettings->Tank2_High_Limit));

			if (strstr(instr, "Tank3_High_Limit"))
				sscanf(datapos, "%d", &(psettings->Tank3_High_Limit));

			if (strstr(instr, "Tank2_Sensor_Limit"))
				sscanf(datapos, "%d", &(psettings->Tank2_Sensor_Limit));

			if (strstr(instr, "Tank3_Sensor_Limit"))
				sscanf(datapos, "%d", &(psettings->Tank3_Sensor_Limit));
		}
	}

	return status;
}

//writes settings to a file
bool WriteSettings(FILE* outfile, SETTINGS* psettings)
{
	int i;
	bool status;
	status = true;

	fprintf(outfile, "\n**Thresholds**\n\n");

	fprintf(outfile, "Min_Treatment_Level  %d\n", psettings->Min_Treatment_Level);
	fprintf(outfile, "Seed_Level  %d\n", psettings->Seed_Level);
	fprintf(outfile, "Settling_time  %d\n", psettings->Settling_time);
	fprintf(outfile, "Recycle_Pcnt  %d\n", psettings->Recycle_Pcnt);
	fprintf(outfile, "Feild_Pcnt  %d\n", psettings->Feild_Pcnt);
	//psettings->URL="http:\\test.com";				// (text Array, > 1K)
	fprintf(outfile, "Idle_Time  %d\n", psettings->Idle_Time);
	fprintf(outfile, "Reporting_Time  %d\n", psettings->Reporting_Time);
	fprintf(outfile, "Empty_Level  %d\n", psettings->Empty_Level);
	fprintf(outfile, "Main_Loop_Time  %d\n", psettings->Main_Loop_Time);
	fprintf(outfile, "Aerator_Count  %d\n", psettings->Aerator_Count);

	fprintf(outfile, "\n**Calibration**\n\n");

	fprintf(outfile, "T_Scale  %d\n", psettings->T_Cal.Scale);
	fprintf(outfile, "T_Offset  %d\n", psettings->T_Cal.Offset);
	fprintf(outfile, "Ph_Scale  %d\n", psettings->Ph_Cal.Scale);
	fprintf(outfile, "Ph_Offset  %d\n", psettings->Ph_Cal.Offset);
	fprintf(outfile, "DO_Scale  %d\n", psettings->DO_Cal.Scale);
	fprintf(outfile, "DO_Offset  %d\n", psettings->DO_Cal.Offset);

	fprintf(outfile, "\n**Error_Limits**\n\n");

	fprintf(outfile, "CPU_Over_Temp  %d\n", psettings->CPU_Over_Temp);
	fprintf(outfile, "Tank_Over_Temp  %d\n", psettings->Tank_Over_Temp);
	fprintf(outfile, "Pump1_Slow_Time  %d\n", psettings->Pump1_Slow_Time);
	fprintf(outfile, "Pump2_Slow_Time  %d\n", psettings->Pump2_Slow_Time);
	fprintf(outfile, "Pump3_Slow_Time  %d\n", psettings->Pump3_Slow_Time);
	fprintf(outfile, "Pump1_Max_Time  %d\n", psettings->Pump1_Max_Time);
	fprintf(outfile, "Pump2_Max_Time  %d\n", psettings->Pump2_Max_Time);
	fprintf(outfile, "Pump3_Max_Time  %d\n", psettings->Pump3_Max_Time);
	fprintf(outfile, "Tank2_High_Limit  %d\n", psettings->Tank2_High_Limit);
	fprintf(outfile, "Tank3_High_Limit  %d\n", psettings->Tank3_High_Limit);
	fprintf(outfile, "Tank2_Sensor_Limit  %d\n", psettings->Tank2_Sensor_Limit);
	fprintf(outfile, "Tank3_Sensor_Limit  %d\n", psettings->Tank3_Sensor_Limit);

	fprintf(outfile, "\n**Lookup_Tables**\n\n");

	for (i = 0; i < AERATOR_BUFSIZE * 2; i++)
	{
		fprintf(outfile, "Aeriation_Duration%d  %d\n", i, psettings->Aeriation_Duration[i]);
	}


	for (unsigned long i = 0; i < 50; i++)
	{
		fprintf(outfile, "Tank2_Level_Lookup%d  %d\n", i, psettings->Tank2_Level_Lookup[i]);
	}


	for (unsigned long i = 0; i < 50; i++)
	{
		fprintf(outfile, "Tank3_Level_Lookup%d  %d\n", i, psettings->Tank3_Level_Lookup[i]);
	}
	

	return status;
}
//reads file modified time and compares to the historical value, which is then reset
//returns whether the current file on disk has the same modification date as the last read
bool FileChanged(char* filename, __time_t* file_time)
{
	bool changed;

	struct stat attrib;
	stat(filename, &attrib);

	changed =*file_time != attrib.st_mtime;

	*file_time = attrib.st_mtime;

	return changed;

}