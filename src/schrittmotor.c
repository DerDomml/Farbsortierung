#include "global.h"


/***************************************
    Definition of Global Variables
***************************************/
bool SMOT_Can_Received = false;
bool bero_delivery;
bool bero_parked;
bool node_listened = 0;
short frequency;
int counted_steps;

/***************************************
        Definition of Methods
***************************************/

bool isBERODelivery(){ return (Data[0] & SMOT_BERO_DELIVERY); }
bool isBEROParked(){ return (Data[0] & SMOT_BERO_PARKED); }



void SMOT_Init(){

    uint8_t d[] = {1,0,0,0,0,0,0,0};
    int transmit_status = 0;

    while(node_listened == false)
    {
        do
        {
            transmit_status = CAN_TransmitMsg(0x00, d, CAN_DLC_2);

            for(int i = 0; i < 600000; i++)
            {

            }

        }
        while(transmit_status <= 2);
    }

    CAN_TransmitMsg(0x00, d, CAN_DLC_2);


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

void SMOT_Node_Listened(uint8_t node_state)
{
    if(node_state == 0x00)
    {
        node_listened = true;
    }
}
