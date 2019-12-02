/****************************************************
	Project Specific Definitions and Declarations
****************************************************/
#ifndef PROJECT_H
#define PROJECT_H

#include "ILI9341_CmdDriver.h"

//********************************
// Declaration of Bits, Type sbit

//********************************
// Declaration of Constants
#define TRUE               1
#define FALSE              0
#define START              1
#define STOP               0
#define HIGH               1
#define LOW                0
#define ON                 1
#define OFF                0
#define ENA                1
#define DISABLE            0
#define ACTIVE             1
#define INACTIVE           0
#define FORWARDS           0x01
#define NUMOFROWS          240


typedef enum cmd
{
    rdMADCTLToggleRGB,
    rdDisplaySeveralData,
    wrDisplayInversion,
    rdChangeDispDataDir,
    wrColourRowsAndText2TFT,
    wrText2TFT,
    wrString1ToTFT,
    wrString2ToTFT,
    noCmd,
} cmd;


//********************************
// Declaration of Global Variables

//************************************
// Method Declaration
extern void ProjectInit( void);

// Präprozessor C
#endif
