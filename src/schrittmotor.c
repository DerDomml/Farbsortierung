#include "global.h"


/***************************************
    Definition of Global Variables
***************************************/

bool ref_delivery_slider;
bool bero_delivery;
bool bero_parked;
bool node_listened = 0;
short frequency;
int counted_steps;

/***************************************
        Definition of Methods
***************************************/

void MotorInit(){

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

//Initialize the motor's control byte with a given one
void SMOT_CB_Init(int CB_Byte)
{
    uint8_t data[] = {CB_Byte};
    CAN_TransmitMsg(SMOT_AI_ID, data, CAN_DLC_3);
}

void SMOT_DI_Changed(uint8_t DI_Byte)
{
    ref_delivery_slider = DI_Byte & SMOT_REF_DELIVER_SLIDER;
    bero_delivery = DI_Byte & SMOT_BERO_DELIVERY;
    bero_parked = DI_Byte & SMOT_BERO_PARKED;
}

void SMOT_AI_Changed(uint8_t* AI_Bytes)
{
    counted_steps = (AI_Bytes[1] << 8) + AI_Bytes[2];
}

void SMOT_Node_Listened(uint8_t node_state)
{
    if(node_state == 0x00)
    {
        node_listened = true;
    }
}
