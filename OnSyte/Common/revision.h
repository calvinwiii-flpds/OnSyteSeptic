#pragma once

#define REVISION "1.4"

//revision history
// 1.2 
// initial release
// 
// 1.3
// added error codes
// added error handling
// added second tank calibration table
// reformated calibration tables to be indexed by pressure in inH20 and to output gallons
// embedded table is 825 DZ concrete tank
// added pressure in inH20 in addition to volume in gallons
// added cleanout step if tank3 level too high
// meanings of settings
//   empty - low level in tank2 ( non zero for tolerance and keep pump submerged
//	 seed_level - low level in tank 3
//   treatment level - volume moved from tank 2 to tank 3 ( pumping starts when tank 2 volume is treatment level to empty
//   recycle percent - remains same. percentage of tank3 volume
// numerous parameters added to settings file
// //revision date in common folder
// 
// 1.4 
// Slave detection of settings file date and auto-reload on change
// all files common between Master and Slave extracted and placed in common folder


