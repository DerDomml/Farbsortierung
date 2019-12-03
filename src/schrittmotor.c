#include "global.h"


/***************************************
    Definition of Global Variables
***************************************/

short ref_delivery_slider;
short bero_delivery;
short bero_parked;
short frequency;
int counted_steps;

/***************************************
        Definition of Methods
***************************************/

void MotorInit(){

    uint8_t d[] = {1,0,0,0,0,0,0,0};

    for(int i=0; i<=6; i++){
        CAN_TransmitMsg(0x00, d, CAN_DLC_2);
    }
}

void SMOT_CB_Init(int CB_Byte)
{
    uint8_t data[] = {CB_Byte};
    //CAN_TransmitMsg(SMOT_AI_BYTES, data, CAN_DLC_3);
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
