#include "global.h"


/***************************************
    Definition of Global Variables
***************************************/
bool ref_slider;
bool bero_delivery;
bool bero_parked;
bool node_listened = 0;

uint8_t do_prev = 0x00;
uint16_t counted_steps;

/***************************************
        Definition of Methods
***************************************/

bool isBERODelivery(){ return (Data[0] & SMOT_BERO_DELIVERY); }
bool isBEROParked(){ return (Data[0] & SMOT_BERO_PARKED); }
bool isRefSlider(){ return (Data[0] & SMOT_REF_DELIVER_SLIDER); }
uint16_t getCountedSteps(){return (Bytes_To_Int(Data[1], Data[2]));}




void SMOT_Init()
{
    while(node_listened == false)
    {
        CAN_NMTConnect();
    }

    CAN_NMTConnect();
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

    case SMOT_STATUS_ID:
        node_listened = true;
    }
}

//Motor Functions
void SMOT_Motor_Start()
{

}

//Belt Functions
void SMOT_Move_Delivery_Belt(direction_t dir)
{
    uint8_t bytes_to_send[1];

    switch(dir)
    {
    case X_POSITIVE:
        bytes_to_send[0] = (SMOT_DEL_BELT_POS | do_prev) ;
        break;

    case X_NEGATIVE:
        bytes_to_send[0] = (SMOT_DEL_BELT_NEG | do_prev);
        break;

    default:
        break;
    }

    CAN_TransmitMsg(SMOT_DO_ID, bytes_to_send, CAN_DLC_1);
}

void SMOT_Move_Parking_Belt(direction_t dir)
{
    uint8_t bytes_to_send[1];

    switch(dir)
    {
    case X_POSITIVE:
        bytes_to_send[0] = SMOT_DEL_BELT_POS;
        break;

    case X_NEGATIVE:
        bytes_to_send[0] = SMOT_DEL_BELT_NEG;
        break;

    default:
        break;
    }

    CAN_TransmitMsg(SMOT_DO_ID, bytes_to_send, CAN_DLC_1);
}

uint16_t Bytes_To_Int(uint8_t msb, uint8_t lsb)
{
    return (msb << 8) + lsb;
}
