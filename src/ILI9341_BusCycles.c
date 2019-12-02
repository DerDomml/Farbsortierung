/*
**              Basic Bus Cycle Functions for ILI9341
**
**********************************************************************/
/*
Hints:
# Modeling the bus cycles according to 7.6.3. 8-bit Parallel MCU Interface (ILI9341Manual 65)
# Implementation of a 8-bit parallel interface together with the necessary control signals using Port E
# To issue a complete sequence you have to use the following scheme
            TFT_StartCycle();
            TFT_Write... or TFT_Read...
            TFT_EndCycle();
# Board Settings
    o DILs of Port E to Floating
    o DIL left top next to uC: Activate BCK LIGHT and BCK PWM
    o To make use of Touch is probably necessary to active READ-X, READ-Y, DRIVEA, DRIVEB
        READ-X and READ-Y is probably an analog valu
    o Making use of the navigation switch requires pullups for the corresponding GPIO-Bits
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
*/

#include "global.h"

enum TFT_Pin
{
	D = 0x00FF,			// ##DA Whatfor
	RDX = 0x0400,		// Read, MCU reads data with the rising edge
	WRX = 0x0800,		// Write, MCU writes data with the rising edge
	DCX = 0x1000,		// Data or Command Select Pin
						// When DCX = 1, data is selected
						// When DCX = 0, command is selected
	CSX = 0x8000,		// Chip Select Input Pin (low active)
	RESX = 0x0100		// Reset, (low active)
};

/*********************************************************************************

	Initialization of Port E for TFT

*********************************************************************************/
void  TFT_InitILI9341() {
	RCC->APB2ENR |= RCC_APB2Periph_GPIOE;

	GPIO_InitTypeDef GPIOE_InitStructure;
	GPIOE_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIOE_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOE_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOE, &GPIOE_InitStructure);

	GPIOE->BSRR = GPIO_Pin_All;
}

/*********************************************************************************

	Interface Functions to model the write and read cycles with the GPIO Port E

*********************************************************************************/
void  TFT_Write8bitToPort(uint8_t P0_7)
{

	if (TFT_IsOutput())
	{
		GPIOE->ODR &= 0xFF00;
		GPIOE->ODR |= P0_7;
	}
}
uint8_t  TFT_Read8bitFromPort()
{

	if (!TFT_IsOutput())
	{
		return (uint8_t) GPIOE->IDR;
	}
}
void  TFT_StartCycle()
{
	GPIOE->ODR = 0x7F00;
}
void  TFT_EndCycle()
{
	GPIOE->ODR = 0xFF00;
}

void TFT_SetOutput() { GPIOE->CRL = 0x33333333; }
void TFT_SetInput() { GPIOE->CRL = 0x44444444; }
bool TFT_IsOutput() { return (GPIOE->CRL == 0x33333333) ? true : false; }

void  TFT_WriteCmd(uint8_t CMD)					    // ILI9341Manual 28
{
	TFT_SetOutput();
	GPIO_WriteBit(GPIOE,  DCX, Bit_RESET);
	GPIO_WriteBit(GPIOE,  WRX, Bit_RESET);
	TFT_Write8bitToPort(CMD);
	__NOP();
	GPIO_WriteBit(GPIOE,  WRX, Bit_SET);
	GPIO_WriteBit(GPIOE,  DCX, Bit_SET);
}
void  TFT_WriteData(uint8_t DATA)					// ILI9341Manual 28
{
	TFT_SetOutput();
	GPIO_WriteBit(GPIOE,  WRX, Bit_RESET);
	TFT_Write8bitToPort(DATA);
	__NOP();
	GPIO_WriteBit(GPIOE,  WRX, Bit_SET);

	TFT_SetOutput();
}
uint32_t   TFT_ReadFirstInvalidData()				// ILI9341Manual 29
{
	return  TFT_ReadData();
}
uint32_t   TFT_ReadData()							// ILI9341Manual 29
{
	TFT_SetInput(); 								//Reading so switch the lower 8 bits of PORT E to INPUT
	GPIO_WriteBit(GPIOE,  WRX, Bit_SET);  		    //always HIGH if read mode
	GPIO_WriteBit(GPIOE,  RDX, Bit_RESET);

	// for (int i = 0; i < 100000; i++) __NOP();
	for (int i = 0; i < 5; i++) __NOP();            // according to __Lib_TFT_Defs.c by MikroElektronika
	GPIO_WriteBit(GPIOE,  RDX, Bit_SET);

	return  TFT_Read8bitFromPort();
}

uint32_t   TFT_ReadLastData()						// ILI9341Manual 29
{
	uint8_t tmpReadValidData = 0;
	TFT_SetInput(); 								//Reading so switch the lower 8 bits of PORT E to INPUT
	GPIO_WriteBit(GPIOE,  WRX, Bit_SET);  		    //always HIGH if read mode
	GPIO_WriteBit(GPIOE,  RDX, Bit_RESET);

	// for (int i = 0; i < 100000; i++) __NOP();	// ##DA Trial and error? Obviously yes!
	for (int i = 0; i < 5; i++) __NOP();            // according to __Lib_TFT_Defs.c by MikroElektronika

	// ##DA Try different orders of access
	GPIO_WriteBit(GPIOE,  CSX, Bit_SET);
	tmpReadValidData = TFT_Read8bitFromPort();
	GPIO_WriteBit(GPIOE,  RDX, Bit_SET);

	return tmpReadValidData;
}
