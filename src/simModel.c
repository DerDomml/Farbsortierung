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
