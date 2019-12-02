#ifndef ILI9342_BusCycles_H__
#define ILI9341_BusCycles_H__
/**
	Copyright 2017 (c)

	@authors: Nicolas Reinema, Mirko Wenzke, Johannes Lüke
	@email:	lueke.johannes@gmail.com

*/

/*	##DA
	The interface is a  8080 MCU 8-bit bus interface I D[7:0] D[7:0],WRX,RDX,CSX,D/CX ?? - look at the board naming
	What is exactly meant with frame memory?
	Where do you switch GPIOE->CRH to output - only within init_ILI9341()?
	What do we have for the board:
		? 65K colour: 16-bit/pixel (RGB 5-6-5 bits input)  (ILI9341Manual 65)
		? 262K colour: 18-bit/pixel (RGB 6-6-6 bits input) --> this version
	Why is 79000 to write all the pixels of the display: 320 x 480 = 153 600 ? --> probably only 320 x 240
	What exactly happens with Idle Mode On: What is meant with the reduction of colour expression?
	What is MADCTL????
	What is meant with this Address_Set_Mode B5 = 0 / 1 ?


	ILI9341_API_FUNCTION_ID
		How do you transmit data via the dispatch table
		What is fn_max?
		Each command obviously needs an entry in the enum?

*/


//#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************************

	Initialization of Port E for TFT

*********************************************************************************/

	/**
		@brief Initializes the necessary PORT the ILI9341 is connected to.
			   It is mandatory that this is the first function to call to use any
			   other functions of this driver. (except startup_ILI9341())

		@return void
	*/
	void				TFT_InitILI9341();

/*********************************************************************************

	Interface Functions to model the write and read cycles with the GPIO Port E

*********************************************************************************/


	/**
		@brief	Writes exactly 8 bit to the data lines connected to the ILI9341
				but only if the connected lines are set to output.
				If this is not case it is up to the hardware what might happen aka
				##DA What is meant by aka
				Undefined Behaviour!!!
				If the chip is robust nothing really bad will happen otherwise
				you might get a short-circuit.
		@return void
	*/
	void				TFT_Write8bitToPort(uint8_t P0_7);


	/**
		@brief	Reads exactly 8 bit from the data lines connected to the ILI9341
				but only if the connected lines are set to Input. If this is not
				case it is up to the hardware what might happen aka
				Undefined Behaviour!!!
				If the chip is robust nothing really bad will happen otherwise
				you might get a short-circuit.

			@return uint8_t
	*/
	uint8_t				TFT_Read8bitFromPort();


	/**
		@brief	Starts ANY command OR data cycle to the ILI9341
				Activates the chip select CSX and switches all data to 0
		@return void
	*/
	void				TFT_StartCycle();


	/**
		@brief	Ends ANY command OR data cycle to the ILI9341
		@return void
	*/
	void				TFT_EndCycle();


	/**
		Those three functions are secret ....
	*/
	void				TFT_SetOutput();
	void				TFT_SetInput();
	bool				TFT_IsOutput();


	/** ILI9341Manual 28
		@brief	Selects a command sent to the ILI9341
		@param	uint8_t	the numeric command (ILI9341Manual 83)
		@return void
	*/
	void				TFT_WriteCmd(uint8_t CMD);


	/** ILI9341Manual 28
		@brief	Selects data sent to the ILI9341
		@param	uint8_t	the numeric command (ILI9341Manual 83)
		@return void
	*/
	void				TFT_WriteData(uint8_t DATA);


	/** ILI9341Manual 29
		@brief	Selects a data sent from the ILI9341
		@param	void
		@return uint32_t
		##DA Why return 32bit from an 8-bit interface?
	*/
	uint32_t			TFT_ReadFirstInvalidData();
	uint32_t			TFT_ReadData();
	uint32_t			TFT_ReadLastData();



#ifdef __cplusplus
}
#endif


#endif // !ILI9342_BusCycles_H__
