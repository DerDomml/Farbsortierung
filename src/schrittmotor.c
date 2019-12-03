#include "global.h"


/***************************************
    Definition of Global Variables
***************************************/
bool SMOT_Can_Received = false;
short ref_delivery_slider;
short bero_delivery;
short bero_parked;
short frequency;
int counted_steps;

/***************************************
        Definition of Methods
***************************************/

bool isBERODelivery(){ return (Data[0] & SMOT_BERO_DELIVERY); }
bool isBEROParked(){ return (Data[0] & SMOT_BERO_PARKED); }



void SMOT_Init(){

    uint8_t d[] = {1,0,0,0,0,0,0,0};

    for(int i=0; i<=6; i++)
    {
        CAN_TransmitMsg(0x00, d, CAN_DLC_2);
    }
}

void SMOT_Tick(){
    if(SMOT_Can_Received){
        if(isBERODelivery()){
            int i = 0;
        }
        if(isBEROParked()){
            int i = 0;
        }

        SMOT_Can_Received = false;
        return;
    }
}
