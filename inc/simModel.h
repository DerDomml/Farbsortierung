/*
**              Staudinger Simulation Model
**					Process Line
**					and CANOpen
**
**********************************************************************/

#ifndef SIMMODEL_H__
#define SIMMODEL_H__


#define CAN_NMTID               0x00
#define SIM_MODEL_ACTOR_DO     	0x201
#define SIM_MODEL_SENSOR_DI    	0x181

#define TFTReframeOn            1
#define TFTReframeOff           0

#define MODEL_DLC               CAN_DLC_1

//-------- CAN-Defines --------------

// Data Output from Process Line


//********************************
// Declaration of Global Variables
bool flagOnholdTransmit;
bool flagRequestTransmitNMT;
bool flagRequestTransmitPL;

// Declaration of Global Functions
// Data Input Functions from Process Line

// Data Output Functions from Process Line

// further SIM functions
extern void SIM_Init();
extern void SIM_Model_ReceiveMsg(CanRxMsg* RxMessage);
extern void SIM_Model_TransmitNewData();


#endif // SIMMODEL_H__
