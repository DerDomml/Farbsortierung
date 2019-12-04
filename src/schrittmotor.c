#include "global.h"


/***************************************
    Definition of Global Variables
***************************************/
bool SMOT_Can_Received = false;
bool ref_slider;
bool bero_delivery;
bool bero_parked;
bool node_listened = 0;
bool motorMsgRecieved = false;
uint16_t frequency;
uint16_t current_position = 0;

/***************************************
        Definition of Methods
***************************************/

bool isBERODelivery(){ return (Data[0] & SMOT_BERO_DELIVERY); }
bool isBEROParked(){ return (Data[0] & SMOT_BERO_PARKED); }
bool isRefSlider(){ return (Data[0] & SMOT_REF_DELIVER_SLIDER); }
//uint16_t getCountedSteps(){return (Bytes_To_Int(Data));}




void SMOT_Init()
{
    uint8_t d[] = {1,0,0,0,0,0,0,0};
    int transmit_status = 0;

    for(int i = 0; i<3; i++)
    {
        CAN_TransmitMsg(0x00, d, CAN_DLC_2);
        for(int k = 0; k < 200000; k++);
    }


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
        motorMsgRecieved = true;
        current_position = uint8s_To_uint16(Data[2], Data[1]);
        break;
    }

    SMOT_Tick();
}

void SMOT_Tick()
{
    if(motorMsgRecieved){
            if(SMOT_ReachedEndPos()){
                    SMOT_Stop();

            }
    }
    motorMsgRecieved = false;
}

void SMOT_Node_Listened(uint8_t node_state)
{
    if(node_state == 0x00)
    {
        node_listened = true;
    }
}



int32_t SMOT_goingToPos = -1;

void SMOT_ResetCounter(){
    uint8_t resetCounterPacket[1] = {0x20};
    CAN_TransmitMsg(SMOT_AO_ID, resetCounterPacket, CAN_DLC_1 );
}

//void SMOT_Goto(uint16_t pos, uint16_t speed, int direction)

void SMOT_Goto(uint16_t pos, uint16_t speed, int direction)
{
    //SMOT_ResetCounter();

    uint8_t directionSetPacket[1] = {(direction << 4)};
    CAN_TransmitMsg(SMOT_DO_ID, directionSetPacket, CAN_DLC_1);

    uint8_t bytesToSend[3] = {0x20, (speed & 0x00FF), ((speed >> 8) & 0xFF)};
    SMOT_goingToPos = pos;
    CAN_TransmitMsg(SMOT_AO_ID, bytesToSend, CAN_DLC_3 );
}

void SMOT_Stop()
{
    uint8_t bytesToSend[3] = {0x00, 0x00, 0x00};
    SMOT_goingToPos = -1;
    CAN_TransmitMsg(SMOT_AO_ID, bytesToSend, CAN_DLC_3 );
}

bool SMOT_ReachedEndPos()
{
    if((current_position >= SMOT_goingToPos) && SMOT_goingToPos != -1){
        return true;
    }
    return false;
}

uint16_t uint8s_To_uint16(uint8_t msb,uint8_t lsb)
{
     uint16_t ret = 0;
    ret |= lsb;
    ret |= msb << 8;

    return ret;
}
