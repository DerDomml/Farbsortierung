#ifndef ILI9341_CmdDriver_H__
#define ILI9341_CmdDriver_H__
/**
	Copyright 2017 (c)

	@authors: Nicolas Reinema, Mirko Wenzke, Johannes Lüke
	@email:	lueke.johannes@gmail.com

	To build new Commands from the Manual the following template must be used.
	Commands without any parameter:

		start_cycle()
		write_cmd(xxx);
		end_cycle();

	Commands with one or n parameter

		start_cycle();
		write_cmd(xxx);
		write_data(yyy);
		...
		end_cycle();

	This driver does not implement all instructions of the ILI9341 but it suffices
	to implement simple graphic routines so for instance text could be displayed.
	(text is not included here!!!)

*/

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum colour
{   red         = 0xff0000,
    green       = 0x00ff00,
    blue        = 0x0000ff,
    cyan        = 0x00ffff,
    margenta    = 0xff00ff,
    yellow      = 0xffff00,
    orange      = 0xff1000,
    white       = 0xffffff,
    black       = 0x000000,
    noSelection    = 0x000000,
} colour;

#define MADCTLHorizontalRGB     0x28

/*********************************************************************************

	Support function for TFT controller ILI9341

*********************************************************************************/

	/**
		Returns the Height of the Display
	*/
	uint16_t TFT_HEIGHT();

	/**
		rns the Width the Display
	*/
	uint16_t TFT_WIDTH();

	/**
		Returns the maximal count of individual pixels
		the TFT-Screen can display at once!
	*/
	uint32_t TFT_MaxPixelCount();


/*********************************************************************************

	Initialization of Port E and the TFT controller ILI9341

*********************************************************************************/

	/**

	*/
	void				TFT_StartupILI9341();


/*********************************************************************************

	Command List ILI9341Manual 83ff - Several commands implemented

*********************************************************************************/
/*********************************************************************************

	Sending to the TFT

*********************************************************************************/
/*********************************************************************************
	void TFT_Function(void)
*********************************************************************************/
	/** ILI9341Manual 89
		@brief	Invokes the NOP instruction on the ILI9341.
				This command is an empty command; it does not have any effect on the display module.
				However it can be used to terminate Frame Memory Write or Read as described in RAMWR
				(Memory Write) and RAMRD (Memory Read) Commands.
		@return void
	*/
	void				TFT_NOP();


	/**	ILI9341Manual 90
	@brief	Invokes the SoftwareReset instruction on the ILI9341.

			When the Software Reset command is written, it causes a software reset.
			It resets the commands and parameters to their S/W Reset default values. (See default tables in each command description.)
			Note: The Frame Memory contents are unaffected by this command

			It will be necessary to wait 5msec before sending new command following software reset.

			##DA Where is the delay to wait?

			The display module loads all display supplier factory default values to the registers
			during this 5msec. If Software Reset is applied during Sleep Out mode, it will be
			necessary to wait 120msec before sending Sleep out command. Software Reset Command
			cannot be sent during Sleep Out sequence.

	@return void
	*/
	void				TFT_SoftwareReset();


	/**	ILI9341Manual 109
		@brief	Invokes the DisplayOn instruction on the ILI9341.

				This command is used to recover from DISPLAY OFF mode.
				Output from the Frame Memory is enabled.
				This command makes no change of contents of frame memory.
				This command does not change any other status.

		@return void
	*/
	void				TFT_DisplayOn();


	/**	ILI9341Manual 108
		@brief	Invokes the DisplayOff instruction on the ILI9341.
				This command is used to enter into DISPLAY OFF mode.
				In this mode, the output from Frame Memory is disabled and blank page inserted.
				This command makes no change of contents of frame memory.
				This command does not change any other status.
				There will be no abnormal visible effect on the display.

		@return void
	*/
	void				TFT_DisplayOff();


	/**	ILI9341Manual 100
		@brief	Invokes the EnterSleepMode instruction on the ILI9341.

				This command causes the LCD module to enter the minimum power consumption mode.
				In this mode e.g. the DC/DC converter is stopped, Internal oscillator is stopped,
				and panel scanning is stopped.

				It is necessary to wait 5msec before sending the next command:
					This waiting time allows the supply voltages and clock circuits to stabilize.

				It will be necessary to wait 120msec after sending Sleep Out command (when in Sleep In Mode)
				before Sleep In command can be sent again.

	@return void
	*/
	void				TFT_EnterSleepMode();

	/**	ILI9341Manual 101
		@brief	Invokes the SleepOut instruction on the ILI9341.

				This command turns off sleep mode.
				In this mode e.g. the DC/DC converter is enabled, Internal oscillator is started, and panel scanning is started.

				This command has no effect when module is already in sleep out mode.
				Sleep Out Mode can only be left by the Sleep In Command (10h).
				It is necessary to wait 5msec before sending the next command:
					This waiting time allows the supply voltages and clock circuits to stabilize.

				The display module loads all display supplier’s factory default values to the registers during this 5msec
				and there cannot be any abnormal visual effect on the display image
				if factory default and register values are same when this load is done
				and when the display module is already Sleep Out –mode.

				The display module is doing self-diagnostic functions during this 5msec.
				It is necessary to wait for 120msec after sending Sleep In command
				(when in Sleep Out Mode) before Sleep Out command can be sent again.

		@return void
	*/
	void				TFT_SleepOut();


	/**	ILI9341Manual 106
		@brief	Invokes the DisplayInversionON instruction on the ILI9341.

				This command is used to enter into display inversion mode.
				This command makes no change of the content of frame memory.
				Every bit is inverted from the frame memory to the display.
				This command doesn’t change any other status.
				To exit Display inversion mode, the Display inversion OFF
				command (20h) is to issued.

		@return void
	*/
	void				TFT_DisplayInversionON();


	/**	ILI9341Manual 105
		@brief	Invokes the DisplayInversionOFF instruction on the ILI9341.

				This command is used to recover from display inversion mode.
				This command makes no change of the content of frame memory.
				This command doesn’t change any other status.

		@return void
	*/
	void				TFT_DisplayInversionOFF();


	/**	ILI9341Manual 132
		@brief	Invokes the IdleModeON instruction on the ILI9341.

				This command is used to enter into Idle mode on.
				In the idle on mode, colour expression is reduced.
				The primary and the secondary colours using MSB of each R, G and B in the Frame Memory, 8 colour depth
				data is displayed.

		@return void
	*/
	void				TFT_IdleModeON();


	/**	ILI9341Manual 131
		@brief	Invokes the IdleModeOFF instruction on the ILI9341.

				This command is used to recover from Idle mode on.
				In the idle off mode, LCD can display maximum 262,144 colours.

		@return void
	*/
	void				TFT_IdleModeOFF();


	/**	ILI9341Manual 127
	@brief	Invokes the Memory Access Control instruction on the ILI9341.

			This command changes Row Address Order and
                carries out a Row/Column Exchange.
			This command makes no change on the other driver status.

	@return void
	*/
	void				TFT_InvertOrient();


/*********************************************************************************
	void TFT_Function(uint8_t param1)
*********************************************************************************/
	/**	ILI9341Manual 127
	@brief	Invokes the Memory Access Control instruction on the ILI9341.

			This command defines read/write scanning direction of frame memory.
			This command makes no change on the other driver status.

	@return void
	*/
    void                TFT_MemoryAccessControl(uint8_t writeMADCTL);


	/**	ILI9341Manual 134
	@brief	Invokes the PixelFormat instruction on the ILI9341.
	@return void
	*/
	void				TFT_PixelFormat(uint8_t pixelFormat);


	/**	ILI9341Manual 154
	@brief	Invokes the RGBInterfaceSignalControl instruction on the ILI9341.
	@return void
	*/
	void				TFT_RGBInterfaceSignalControl(uint8_t crtlRGB);


/*********************************************************************************
	void TFT_Function(uint16_t param1, uint16_t param2)
*********************************************************************************/
	/**	ILI9341Manual 110
		@brief	Invokes the ColumnAddressSet instruction on the ILI9341.

				This command is used to define area of frame memory where MCU can access.
				This command makes no change on the other driver status.
				The values of SC [15:0] and EC [15:0] are referred when RAMWR command comes.
				Each value represents one column line in the Frame Memory.


				 SC		 EC
				 |		  |
				############
				####+++#####
				###+###+####
				######++####
				####+##+####
				###+###+####
				####+++##+##
				############

				SC [15:0] always must be equal to or less than EC [15:0].
				Note 1:
				When SC [15:0] or EC [15:0] is greater
				than 00EFh (when MADCTL’s B5 = 0) or 013Fh (when MADCTL’s B5 = 1),
				data of out of range will be ignored


		@return void
	*/
	void				TFT_ColumnAddressSet(uint16_t SC, uint16_t EC);


	/**	ILI9341Manual 112
		@brief	Invokes the PageAddressSet instruction on the ILI9341.

				This command is used to define area of frame memory where MCU can access.
				This command makes no change on the other driver status.
				The values of SP [15:0] and EP [15:0] are referred when	RAMWR command comes.
				Each value represents one Page line in the Frame Memory.

			  SP->############
				  ####+++#####
				  ###+###+####
				  ######++####
				  ####+##+####
				  ###+###+####
				  ####+++##+##
			  EP->############

			  SP [15:0] always must be equal to or less than EP [15:0]
			  Note 1:
			  When SP [15:0] or EP [15:0] is greater
			  than 013Fh (when MADCTL’s B5 = 0)
			  or 00EFh (when MADCTL’s B5 = 1),
			  data of out of range will be ignored.

		@return void
	*/
	void				TFT_PageAddressSet(uint16_t SP, uint16_t EP);

	/**	ILI9341Manual 110 + 112
	@brief	Invokes both driver commands ColumnAddressSet and PageAddressSet to reframe the TFT
	@brief	It differs between horizontal and vertical frames
	@return void
	*/
    void TFT_ReframeAddrSet(uint16_t leftPos, uint16_t rightPos, uint16_t topPos, uint16_t bottomPos);

/*********************************************************************************
	void TFT_Function(uint32_t param1)
*********************************************************************************/

    void TFT_DisplayClearWithColour(uint32_t selColour);


/*********************************************************************************
	void TFT_Function(uint32_t param1, uint32_t param2)
*********************************************************************************/
	/**	ILI9341Manual 114
		@brief	Invokes the MemoryWrite instruction on the ILI9341.

				This command is used to transfer data from MCU to frame memory.
				This command makes no change to the other driver status.
				When this command is accepted, the column register and the page
				register are reset to the Start Column/Start Page positions.

				The Start Column/Start Page  positions are different in accordance with MADCTL setting.
				Then D [17:0] is stored in the frame memory
				and the column register and the page register are incremented.
				Sending any other command can stop frame Write.

		@param	uint32_t data[32:0]: Cyan[20:15] Magenta[13:8] Yellow[5:0]
				##DA probably Cyan[21:16]
		@param  uint32_t nmbOfPixel: Number of pixel writes to the display.
		@return void
	*/
	void				TFT_MemoryWrite(uint32_t data, uint32_t nmbOfPixel);


	/**	ILI9341Manual 135
		@brief	Invokes the MemoryWriteContinue instruction on the ILI9341.

				This command transfers image data from the host processor to the display module’s frame memory
				continuing from the pixel location following the previous write_memory_continue or write_memory_start
				command.

				If set_address_mode B5 = 0:

				Data is written continuing from the pixel location after the
				write range of the previous write_memory_start or write_memory_continue. The column register is then
				incremented and pixels are written to the frame memory until the column register equals
				the End Column (EC) value. The column register is then reset to SC and the page register is incremented.
				Pixels are written to the frame memory until the page register equals the End Page (EP) value and
				the column register equals the EC value, or the host processor sends another command. If the number
				of pixels exceeds (EC –SC + 1) * (EP – SP + 1) the extra pixels are ignored.

				If set_address_mode B5 = 1:

				Data is written continuing from the pixel location after the write range of the previous
				write_memory_start or write_memory_continue. The page register is then incremented and pixels are
				written to the frame memory until the page register equals the End Page (EP) value. The page register
				is then reset to SP and the column register is incremented. Pixels are written to the frame memory until
				the column register equals the End column (EC) value and the page register equals the EP value, or
				the host processor sends another command. If the number of pixels exceeds (EC – SC + 1) * (EP – SP + 1)
				the extra pixels are ignored.

				Sending any other command can stop frame Write.
				Frame Memory Access and Interface setting (B3h), WEMODE=0 When the transfer number of data exceeds (EC-SC+1)*(EP-SP+1), the exceeding data will be ignored.
				Frame Memory Access and Interface setting (B3h), WEMODE=1 When the transfer number of data exceeds (EC-SC+1)*(EP-SP+1), the column and page number will be reset, and the
				exceeding data will be written into the following column and page.


				A write_memory_start should follow a set_column_address, set_page_address or set_address_mode
				to define the write address. Otherwise, data written with write_memory_continue is written to
				undefined addresses.

		@param	uint32_t data[32:0]: Cyan[20:15] Magenta[13:8] Yellow[5:0]
		@param  uint32_t nmbOfPixel: Number of pixel writes to the display.

		@return void
	*/
	void				TFT_MemoryWriteContinue(uint32_t data, uint32_t nmbOfPixel);

/*********************************************************************************

	Reading from the TFT

*********************************************************************************/
/*********************************************************************************
	uint32_t TFT_Function(void)
*********************************************************************************/
    /** ILI9341Manual 91
    This read byte returns 24 bits display identification information.
    The 1st parameter is dummy data.
    The 2nd parameter (ID1 [7:0]): LCD module’s manufacturer ID.
    The 3rd parameter (ID2 [7:0]): LCD module/driver version ID.
    The 4th parameter (ID3 [7:0]): LCD module/driver ID.
    **/
    uint32_t            TFT_ReadDisplayIdentInfo();


    /** ILI9341Manual 92
    This command indicates the current status of the display as
    described in the table on page 92
    It returns four relevant bytes
    **/
    uint32_t            TFT_ReadDisplayStatus();


    /** ILI9341Manual 94
    This command indicates the current status of the display as
    described in the table on page 94
    It returns one relevant bytes
    **/
    uint32_t            TFT_ReadDisplayPowerMode();


    /** ILI9341Manual 95
    This command indicates the current status of the display as
    described in the table on page 95
    It returns one relevant bytes
    **/
    uint32_t            TFT_ReadDisplayMADCTL();


    /** ILI9341Manual 96
    This command indicates the current status of the display as
    described in the table on page 96
    It returns one relevant bytes
    **/
    uint32_t            TFT_ReadDisplayPixelFormat();


    /** ILI9341Manual 97
    This command indicates the current status of the display as
    described in the table on page 97
    It returns one relevant bytes
    **/
    uint32_t            TFT_ReadDisplayImageFormat();


    /** ILI9341Manual 98
    This command indicates the current status of the display as
    described in the table on page 98
    It returns one relevant bytes
    **/
    uint32_t            TFT_ReadDisplaySignalMode();


    /** ILI9341Manual 99
    This command indicates the current status of the display as
    described in the table on page 99
    It returns one relevant bytes / basically two bits
    **/
    uint32_t            TFT_ReadDisplaySelfDiagnosticResult();


/*********************************************************************************

	API to control the TFT

*********************************************************************************/
	/*
		This part of the driver is more a gimmick then anything else.
		You also need a better understanding of C to really use this.
		Nothing impossible but it's definitely a tough topic for a beginner.
		If you have any questions about I am happy to answer them all.

		sincerely

		Johannes Lueke

        ------------------------------

		This array stores pointers to all functions which have been implemented so far.
		An array which stores pointers to functions is often called a dispatch tables.
		Depending which function you choose via the array index
			you can change the behaviour of your program dynamically.

		Example:

			int main()
			{
				((void(*)()) TFT_CmdTable[cmd_TFT_STARTUPILI9341])();
				((void(*)()) TFT_CmdTable[cmd_TFT_SOFTWARERESET])();
				((void(*)()) TFT_CmdTable[cmd_TFT_SLEEPOUT])();
				((void(*)()) TFT_CmdTable[cmd_TFT_DISPLAYON])();
				((void(*)()) TFT_CmdTable[cmd_TFT_DISPLAYINVERSIONON])();

			  for (;;)
				  ((void(*)(uint32_t,uint32_t)) TFT_CmdTable[ILI9341_API_CMD_ID::cmd_TFT_MEMORYWRITE])(0xff'66'00, 79000);

			 }

			 The call of a function is now a bit verbose but this verbosity is a small prize compared
			 to what you will win on functionality.


			 In the enumeration ILI9341_API_CMD_ID all indices are named corresponding
			 the appropriate function call. Instead of those symbols it is possible to simply use
			 any integer in the same way you are used to within arrays

			 e.g.
			 ((void(*)()) TFT_CmdTable[cmd_TFT_STARTUPILI9341])()
				is equal to
			((void(*)()) TFT_CmdTable[0])()

			instead of writing (void(*)()) to cast your stored pointer to a correct function it is possible
			to create a new type by writing:

			typedef void(*)()	Func;

			which would enable us to write than:

			((Func) TFT_CmdTable[0])()

	*/
	/* enum ILI9341_API_FUNCTION_ID {
		fn_INIT_ILI9341,
		fn_SETOUTPUT,
		fn_SETINPUT,
		fn_ISOUTPUT,
		fn_WRITE_8BIT_TO_PORT,
		fn_READ_8BIT_FROM_PORT,
		fn_START_CYCLE,
		fn_END_CYCLE,
		fn_WRITE_CMD,
		fn_WRITE_DATA,
		fn_TFT_READ_DATA,
		fn_TFT_NOP,
		fn_TFT_SOFTWARERESET,
		fn_TFT_DISPLAYON,
		fn_TFT_DISPLAYOFF,
		fn_TFT_ENTERSLEEPMODE,
		fn_TFT_SLEEPOUT,
		fn_TFT_DISPLAYINVERSIONON,
		fn_TFT_DISPLAYINVERSIONOFF,
		fn_TFT_IDLEMODEON,
		fn_TFT_IDLEMODEOFF,
		fn_TFT_COLUMNADDRESSSET,
		fn_TFT_PAGEADDRESSSET,
		fn_TFT_MEMORYWRITE,
		fn_TFT_MEMORYWRITECONTINUE,
		fn_TFT_PIXELFORMAT,
		fn_TFT_RGBINTERFACESIGNALCONTROL,
		fn_TFT_INVERTORIENT,
		fn_MAX
	};
	extern void* TFT_DispatchTable[fn_MAX];
	*/

	enum ILI9341_API_CMD_ID {
		// void TFT_Function(void)
		cmd_TFT_STARTUPILI9341,
		cmd_TFT_NOP,
		cmd_TFT_SOFTWARERESET,
		cmd_TFT_DISPLAYON,
		cmd_TFT_DISPLAYOFF,
		cmd_TFT_ENTERSLEEPMODE,
		cmd_TFT_SLEEPOUT,
		cmd_TFT_DISPLAYINVERSIONON,
		cmd_TFT_DISPLAYINVERSIONOFF,
		cmd_TFT_IDLEMODEON,
		cmd_TFT_IDLEMODEOFF,
		cmd_TFT_INVERTORIENT,

		// void TFT_Function(uint8_t param)
		cmd_TFT_MEMORYACCESSCONTROL,
		cmd_TFT_PIXELFORMAT,
		cmd_TFT_RGBINTERFACESIGNALCONTROL,

		// void TFT_Function(uint16_t param1, uint16_t param2)
		cmd_TFT_COLUMNADDRESSSET,
		cmd_TFT_PAGEADDRESSSET,

        // void TFT_Function(uint32_t param1)
        cmd_TFT_DISPLAYCLEAR_WITHCOLOUR,

		// void TFT_Function(uint32_t param1, uint32_t param2)
		cmd_TFT_MEMORYWRITE,
		cmd_TFT_MEMORYWRITECONTINUE,

		// uint32_t TFT_Function(void)
		cmd_TFT_READDISPLAY_IDENTINFO,
		cmd_TFT_READDISPLAY_STATUS,
		cmd_TFT_READDISPLAY_POWERMODE,
		cmd_TFT_READDISPLAY_MACCTL,
		cmd_TFT_READDISPLAY_PIXELFORMAT,
		cmd_TFT_READDISPLAY_IMAGEFORMAT,
		cmd_TFT_READDISPLAY_SIGNALMODE,
		cmd_TFT_READDISPLAY_SELFDIAGNOSTICRESULT,

        // void TFT_Function(uint16_t param1, uint16_t param2, uint16_t param3, uint16_t param4)
        cmd_TFT_REFRAMEADDRSET,
		cmd_MAX
	};
	extern void* TFT_CmdTable[cmd_MAX];

    // to cast the void-pointer to a two parameter function
	typedef void TFT_Cmd2Param (uint32_t, uint32_t);
	// to cast the void-pointer to a void function
	typedef void TFT_CmdVoid();
    // to cast the void-pointer to a two parameter function
	typedef void TFT_Cmd4Param (uint16_t, uint16_t, uint16_t, uint16_t);

	// typedef uint32_t TFTCALL (void* array, uint16_t sizeOfArray);       // generic type of the TFT functions


    #define noTFTCmd    -1

#ifdef __cplusplus
}
#endif


#endif // !ILI9341_CmdDriver_H__
