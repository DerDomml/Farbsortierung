/*
**            Set of Driver Functions for TFT / ILI9341
**
**********************************************************************/

/**********************************************************************
Some hints:
1. The orientation seems to be 240 x 320 therefore a vertical orientation
    Therefore 240 is the given width while 320 is the given height
2. Default is BGR with Display Inversion Off (with glossy displays, not the matte versions)
3. After reframing the display MemoryWrite will start in the left top corner
    (depending on the orientation)
    while MemoryWriteContinue just continues at the last pixel position
    (Be careful with MemoryWriteContinue after writing strings to the TFT)

**********************************************************************/
/** Necessary includes for a stand alone driver
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include "ILI9341_BusCycles.h"
#include "ILI9341_CmdDriver.h"
*/

#include "global.h"

// Command list: ILI9341 83
// Hint: 	Not giving the explicit number works with enum because it just
// 			uses the following number in the numeric order
enum TFT_Command
{
	NOP = 0x00,
	SWRESET,
	RDDIDIF = 0x04,
	RDDST = 0x09,
	RDDPM,
	RDDMADCTL,
	RDDCOLMOD,
	RDDIM,
	RDDSM,
	RDDSDR,
	ENTERSLEEPMODE = 0x10,
	LEAVESLEEPMODE,
	DISPLAYINVERSION_OFF = 0x20,
	DISPLAYINVERSION_ON,
	DISPLAYOFF = 0x28,
	DISPLAYON,
	COLUMNADDRESS_SET = 0x2a,
	PAGEADDRESS_SET,
	MEMORYWRITE,
	MEMORYACCESS_CONTROL = 0x36,
	IDLEMODE_OFF = 0x38,
	IDLEMODE_ON,
	PIXELFORMAT = 0x3a,
	MEMORYWRITE_CONTINUE = 0x3c,
	RGB_INTERFACE_SIGNAL_CONTROL = 0xb0,
};

static const uint16_t ILI9341_WIDTH = 240-1;
static const uint16_t ILI9341_HEIGHT = 320-1;
static const uint32_t ILI9341_PIXEL = 76800;

uint16_t TFT_HEIGHT() {
	return ILI9341_HEIGHT;
}

uint16_t TFT_WIDTH() {
	return ILI9341_WIDTH;
}

uint32_t TFT_MaxPixelCount() {
	return ILI9341_PIXEL;
}


/*********************************************************************************

	Initialization of Port E and the TFT controller ILI9341

*********************************************************************************/
void TFT_StartupILI9341() {
	TFT_InitILI9341();
	TFT_SoftwareReset();
	TFT_DisplayOn();
	TFT_SleepOut();
	TFT_DisplayInversionOFF();
	TFT_MemoryAccessControl(MADCTLHorizontalRGB);
	TFT_ReframeAddrSet(0, TFT_HEIGHT(), 0, TFT_WIDTH());
}

/*********************************************************************************

	Command List ILI9341Manual 83ff - Several commands implemented

*********************************************************************************/
/*********************************************************************************

	Sending to the TFT

*********************************************************************************/
/*********************************************************************************
	void TFT_Function(void)
*********************************************************************************/
void  TFT_NOP()									        // ILI9341Manual 89
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( NOP));
	TFT_EndCycle();
}
void  TFT_SoftwareReset()							    // ILI9341Manual 90
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( SWRESET));
	TFT_EndCycle();
}
void  TFT_DisplayOn()								    // ILI9341Manual 109
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( DISPLAYON));
	TFT_EndCycle();
}
void  TFT_DisplayOff()								    // ILI9341Manual 108
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( DISPLAYOFF));
	TFT_EndCycle();
}
void  TFT_EnterSleepMode()							    // ILI9341Manual 100
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( ENTERSLEEPMODE));
	TFT_EndCycle();
}
void  TFT_SleepOut()								    // ILI9341Manual 101
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( LEAVESLEEPMODE));
	TFT_EndCycle();
}
void  TFT_DisplayInversionON()						    // ILI9341Manual 106
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( DISPLAYINVERSION_ON));
	TFT_EndCycle();
}
void  TFT_DisplayInversionOFF()						    // ILI9341Manual 105
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( DISPLAYINVERSION_OFF));
	TFT_EndCycle();
}
void  TFT_IdleModeON()								    // ILI9341Manual 132
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( IDLEMODE_ON));
	TFT_EndCycle();
}
void  TFT_IdleModeOFF()								    // ILI9341Manual 131
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( IDLEMODE_OFF));
	TFT_EndCycle();
}
void  TFT_InvertOrient()							    // ILI9341Manual 127
{
	TFT_StartCycle();
	TFT_WriteCmd(MEMORYACCESS_CONTROL);
	TFT_WriteData(0b10100000);
	TFT_EndCycle();
}
/*********************************************************************************
	void TFT_Function(uint8_t param1)
*********************************************************************************/
void TFT_MemoryAccessControl(uint8_t writeMADCTL)	    // ILI9341Manual 127
{
	TFT_StartCycle();
	TFT_WriteCmd(MEMORYACCESS_CONTROL);
	TFT_WriteData(writeMADCTL);
	TFT_EndCycle();
}
void  TFT_PixelFormat(uint8_t pixelFormat)				// ILI9341Manual  134
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t) PIXELFORMAT);
	TFT_WriteData(pixelFormat);
	TFT_EndCycle();
}
void  TFT_RGBInterfaceSignalControl(uint8_t crtlRGB)	// ILI9341Manual 154
{
	TFT_StartCycle();
	TFT_WriteCmd(RGB_INTERFACE_SIGNAL_CONTROL);
	TFT_WriteData(crtlRGB);
	TFT_EndCycle();
}
/*********************************************************************************
	void TFT_Function(uint16_t param1, uint16_t param2)
*********************************************************************************/
void  TFT_ColumnAddressSet(uint16_t SC, uint16_t EC)	// ILI9341Manual 110
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( COLUMNADDRESS_SET));
	TFT_WriteData((uint8_t)(SC >> 8));
	TFT_WriteData((uint8_t)(SC));
	TFT_WriteData((uint8_t)(EC >> 8));
	TFT_WriteData((uint8_t)(EC));
	TFT_EndCycle();
}
void  TFT_PageAddressSet(uint16_t SP, uint16_t EP)	    // ILI9341Manual 112
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( PAGEADDRESS_SET));
	TFT_WriteData((uint8_t)(SP >> 8));
	TFT_WriteData((uint8_t)(SP));
	TFT_WriteData((uint8_t)(EP >> 8));
	TFT_WriteData((uint8_t)(EP));
	TFT_EndCycle();

}
void TFT_ReframeAddrSet(uint16_t leftPos, uint16_t rightPos, uint16_t topPos, uint16_t bottomPos)
{
    // horizontal
    if (TFT_ReadDisplayMADCTL() & 0x20)
    {
        TFT_ColumnAddressSet(leftPos, rightPos);
        TFT_PageAddressSet(topPos, bottomPos);
    }
    // vertical
    else
    {
        TFT_ColumnAddressSet(topPos, bottomPos);
        TFT_PageAddressSet(leftPos, rightPos);
    }
}
//data[32:0]: Cyan[20:15] Magenta[13:8] Yellow[5:0] if DisplayInversionOn
//##DA it depends on the board
/*********************************************************************************
	void TFT_Function(uint32_t param1, uint32_t param2)
*********************************************************************************/
void  TFT_MemoryWrite(uint32_t data, uint32_t nmbOfPixel)   // ILI9341Manual 114
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( MEMORYWRITE));
	for (int i = 0; i < nmbOfPixel; i++)
	{

		TFT_WriteData((uint8_t)((data >> 14))); 	    // starts with the MSB
		TFT_WriteData((uint8_t)((data >> 6)));
		TFT_WriteData((uint8_t)(data << 2));
	}
	TFT_EndCycle();
}
void  TFT_MemoryWriteContinue(uint32_t data, uint32_t nmbOfPixel)	// ILI9341Manual 135
{
	TFT_StartCycle();
	TFT_WriteCmd((uint8_t)( MEMORYWRITE_CONTINUE));
	for (int i = 0; i < nmbOfPixel; i++)
	{

		TFT_WriteData((uint8_t)((data >> 14))); 	    // starts with the MSB
		TFT_WriteData((uint8_t)((data >> 6)));
		TFT_WriteData((uint8_t)(data << 2));

	}
	TFT_EndCycle();
}
void TFT_DisplayClearWithColour(uint32_t selColour)
{
    TFT_ReframeAddrSet(0, TFT_HEIGHT(), 0, TFT_WIDTH());
    TFT_MemoryWrite(selColour, (320*240));
    TFT_ReframeAddrSet(0, TFT_HEIGHT(), 0, TFT_WIDTH());
}
/*********************************************************************************

	Reading from the TFT

*********************************************************************************/
/*********************************************************************************
	uint32_t TFT_Function(void)
*********************************************************************************/
uint32_t  TFT_ReadDisplayIdentInfo()   			        // ILI9341Manual 91
{
	uint32_t tmpReadData = 0x12345678;
	TFT_StartCycle();
	TFT_WriteCmd(RDDIDIF);
	TFT_ReadFirstInvalidData();
	tmpReadData = ((TFT_ReadData() & 0x000000ff) << 16);
	tmpReadData = tmpReadData + ((TFT_ReadData() & 0x000000ff) << 8);
	tmpReadData = tmpReadData + (TFT_ReadLastData() & 0x000000ff);
	TFT_EndCycle();
	return tmpReadData;
}
uint32_t  TFT_ReadDisplayStatus()   			        // ILI9341Manual 92
{
	uint32_t tmpReadData = 0xabcdef01;
	TFT_StartCycle();
	TFT_WriteCmd(RDDST);
	TFT_ReadFirstInvalidData();
	tmpReadData = ((TFT_ReadData() & 0x000000ff) << 24);
	tmpReadData = tmpReadData + ((TFT_ReadData() & 0x000000ff) << 16);
	tmpReadData = tmpReadData + ((TFT_ReadData() & 0x000000ff) << 8);
	tmpReadData = tmpReadData + (TFT_ReadLastData() & 0x000000ff);
	TFT_EndCycle();
	return tmpReadData;
}
uint32_t  TFT_ReadDisplayPowerMode()   			        // ILI9341Manual 94
{
	uint32_t tmpReadData = 0x87654321;
	TFT_StartCycle();
	TFT_WriteCmd(RDDPM);
	TFT_ReadFirstInvalidData();
	tmpReadData = (TFT_ReadLastData() & 0x000000ff);
	TFT_EndCycle();
	return tmpReadData;
}
uint32_t  TFT_ReadDisplayMADCTL()   			        // ILI9341Manual 95
{
	uint32_t tmpReadData = 0x12345678;
	TFT_StartCycle();
	TFT_WriteCmd(RDDMADCTL);
	tmpReadData = ((TFT_ReadFirstInvalidData() & 0x000000ff) << 8);
	tmpReadData = tmpReadData + (TFT_ReadLastData() & 0x000000ff);
	TFT_EndCycle();
	return tmpReadData;
}
uint32_t  TFT_ReadDisplayPixelFormat()   		        // ILI9341Manual 96
{
	uint32_t tmpReadData = 0x12345678;
	TFT_StartCycle();
	TFT_WriteCmd(RDDCOLMOD);
	tmpReadData = ((TFT_ReadFirstInvalidData() & 0x000000ff) << 8);
	tmpReadData = tmpReadData + (TFT_ReadLastData() & 0x000000ff);
	TFT_EndCycle();
	return tmpReadData;
}
uint32_t  TFT_ReadDisplayImageFormat()   		        // ILI9341Manual 97
{
	uint32_t tmpReadData = 0x12345678;
	TFT_StartCycle();
	TFT_WriteCmd(RDDIM);
	tmpReadData = ((TFT_ReadFirstInvalidData() & 0x000000ff) << 8);
	tmpReadData = tmpReadData + (TFT_ReadLastData() & 0x000000ff);
	TFT_EndCycle();
	return tmpReadData;
}
uint32_t  TFT_ReadDisplaySignalMode()   		        // ILI9341Manual 98
{
	uint32_t tmpReadData = 0x12345678;
	TFT_StartCycle();
	TFT_WriteCmd(RDDSM);
	tmpReadData = ((TFT_ReadFirstInvalidData() & 0x000000ff) << 8);
	tmpReadData = tmpReadData + (TFT_ReadLastData() & 0x000000ff);
	TFT_EndCycle();
	return tmpReadData;
}
uint32_t  TFT_ReadDisplaySelfDiagnosticResult()         // ILI9341Manual 99
{
	uint32_t tmpReadData = 0x12345678;
	TFT_StartCycle();
	TFT_WriteCmd(RDDSDR);
	tmpReadData = ((TFT_ReadFirstInvalidData() & 0x000000ff) << 8);
	tmpReadData = tmpReadData + (TFT_ReadLastData() & 0x000000ff);
	TFT_EndCycle();
	return tmpReadData;
}

/*********************************************************************************

	API to control the TFT

*********************************************************************************/
/*

void* TFT_DispatchTable[] = {
	(void*)TFT_InitILI9341,
	(void*)TFT_NOP,
	(void*)TFT_SoftwareReset,
	(void*)TFT_DisplayOn,
	(void*)TFT_DisplayOff,
	(void*)TFT_EnterSleepMode,
	(void*)TFT_SleepOut,
	(void*)TFT_DisplayInversionON,
	(void*)TFT_DisplayInversionOFF,
	(void*)TFT_IdleModeON,
	(void*)TFT_IdleModeOFF,
	(void*)TFT_ColumnAddressSet,
	(void*)TFT_PageAddressSet,
	(void*)TFT_MemoryWrite,
	(void*)TFT_MemoryWriteContinue,
	(void*)TFT_PixelFormat,
	(void*)TFT_RGBInterfaceSignalControl,
	(void*)TFT_InvertOrient
};
*/

void* TFT_CmdTable[] = {
	// void TFT_Function(void)
	(void*)TFT_StartupILI9341,
	(void*)TFT_NOP,
	(void*)TFT_SoftwareReset,
	(void*)TFT_DisplayOn,
	(void*)TFT_DisplayOff,
	(void*)TFT_EnterSleepMode,
	(void*)TFT_SleepOut,
	(void*)TFT_DisplayInversionON,
	(void*)TFT_DisplayInversionOFF,
	(void*)TFT_IdleModeON,
	(void*)TFT_IdleModeOFF,
	(void*)TFT_InvertOrient,

    // void TFT_Function(uint8_t param)
    (void*)TFT_MemoryAccessControl,
	(void*)TFT_PixelFormat,
	(void*)TFT_RGBInterfaceSignalControl,

	// void TFT_Function(uint16_t param1, uint16_t param2)
	(void*)TFT_ColumnAddressSet,
	(void*)TFT_PageAddressSet,

    // void TFT_Function(uint32_t param1)
    (void*) TFT_DisplayClearWithColour,

	// void TFT_Function(uint32_t param1, uint32_t param2)
	(void*)TFT_MemoryWrite,
	(void*)TFT_MemoryWriteContinue,

	// uint32_t TFT_Function(void)
	(void*)TFT_ReadDisplayIdentInfo,
	(void*)TFT_ReadDisplayStatus,
	(void*)TFT_ReadDisplayPowerMode,
	(void*)TFT_ReadDisplayMADCTL,
	(void*)TFT_ReadDisplayPixelFormat,
	(void*)TFT_ReadDisplayImageFormat,
	(void*)TFT_ReadDisplaySignalMode,
	(void*)TFT_ReadDisplaySelfDiagnosticResult,

	// void TFT_Function(uint16_t param1, uint16_t param2, uint16_t param3, uint16_t param4)
	(void*)TFT_ReframeAddrSet,
};
