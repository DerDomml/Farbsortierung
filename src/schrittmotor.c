#include "global.h"


/***************************************
    Definition of Global Variables
***************************************/
bool SMOT_Can_Received = false;
bool ref_slider;
bool bero_delivery;
bool bero_parked;
bool node_listened = 0;
uint16_t frequency;
uint16_t counted_steps;

/***************************************
        Definition of Methods
***************************************/

bool isBERODelivery(){ return (Data[0] & SMOT_BERO_DELIVERY); }
bool isBEROParked(){ return (Data[0] & SMOT_BERO_PARKED); }
bool isRefSlider(){ return (Data[0] & SMOT_REF_DELIVER_SLIDER); }
uint16_t getCountedSteps(){return (Bytes_To_Int(Data));}




void SMOT_Init()
{
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

void SMOT_Update()
{
    switch(Msg_ID)
    {
    case SMOT_DI_ID:
        bero_delivery = isBERODelivery();
        bero_parked = isBEROParked();
        ref_slider = isRefSlider();
        break;

    case SMOT_AI_ID:
        break;
    }
}

void SMOT_Node_Listened(uint8_t node_state)
{
    if(node_state == 0x00)
    {
        node_listened = true;
    }
}

void SMOT_Motor_Start()
{

}

uint16_t Bytes_To_Int(uint8_t toSwap[2])
{
    return (toSwap[1] << 4) + toSwap[0];
}

//Trim Array
