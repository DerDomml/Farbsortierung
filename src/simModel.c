/*
**              Staudinger Simulation Model
**
**					and CANOpen
**
**********************************************************************/

#include "global.h"


// file global variables for Simulation Model
uint8_t Model_ReceiveData[] = {0x00};
uint8_t Model_TransmitData[] = {0x00, 0x00, 0x00};

/*********************************************

    Checking the sensor bits

*********************************************/

bool PL_CheckBlockAtWorkstation(){
    return (Data[0] & 0x08) ? true : false;
}

bool PL_CheckBlockAtDump(){
    return (Data[0] & 0x10) ? true : false;
}

bool PL_CheckBlockAtRegister(){
    return (Data[0] & 0x04) ? true : false;
}

bool PL_CheckPistonAtBack(){
    return (Data[0] & 0x01) ? true : false;
}

bool PL_CheckPistonAtRegister(){
    return (Data[0] & 0x12) ? true : false;
}

bool PL_CheckImpulsAtConveyor(){
    return (Data[0] & 0x20) ? true : false;
}

bool PL_CheckControlKey(){
    return (Data[0] & 0x40) ? true : false;
}

/*********************************************

    Set / Reset the Actor bits

*********************************************/





/*********************************************

    Text Messages

*********************************************/
void SIM_Model_Manual()
{
    TFT_ReframeAddrSet(0, TFT_HEIGHT(), 0, TFT_WIDTH());
    TFT_MemoryWrite(red, (NUMOFROWS * 20));
    TFT_MemoryWriteContinue(white, (NUMOFROWS * 280));
    TFT_MemoryWriteContinue(yellow, (NUMOFROWS * 20));
    TFT_ReframeAddrSet(0, TFT_HEIGHT(), 0, TFT_WIDTH());
    TFT_TextPuts("Successful Initialization & NMT-Transmission\n", &Font_7x10, yellow, red);
}

/*********************************************

    Initialization of Simulation Model

*********************************************/
void SIM_Init()
{

}
/*********************************************
    Send Message if new TxData available
*********************************************/
void SIM_Model_TransmitNewData()
{

}

/*********************************************

    Service the Interrupts of the Simulation Model

*********************************************/
void SIM_Model_ReceiveMsg(CanRxMsg* RxMessage)
{

}
