#include "global.h"

//Testcomment
/***************************************
    Definition of Global Variables
***************************************/
bool ref_slider;
bool bero_delivery;
bool bero_parked;
bool node_listened = 0;
bool ENT_Ready_To_Give = false;

uint8_t do_prev = 0x00;
bool motorMsgRecieved = false;
uint16_t frequency;
uint16_t current_position = 0;

bool SMOT_Motor_running = false;
bool SMOT_Motor_stopping = false;
bool SMOT_shall_stop = false;

/***************************************
        Definition of Methods
***************************************/

bool isBERODelivery() { return (Data[0] & SMOT_BERO_DELIVERY); }
bool isBEROParked() { return (Data[0] & SMOT_BERO_PARKED); }
bool isRefSlider() { return (Data[0] & SMOT_REF_DELIVER_SLIDER); }
bool isSMOTStopping() { return (Data[0] & SMOT_AI_STOPPING); }

void SMOT_Init()
{
    CAN_NMTConnect();
}

void SMOT_Update()
{
    switch (Msg_ID)
    {
    case SMOT_DI_ID:
        bero_delivery = isBERODelivery();
        bero_parked = isBEROParked();
        ref_slider = isRefSlider();
        break;

    case SMOT_AI_ID:
        node_listened = true;
        if (SMOT_Motor_stopping && Data[0] == 0x00)
        {
            SMOT_Motor_running = false;
        }
        motorMsgRecieved = true;
        current_position = uint8s_To_uint16(Data[2], Data[1]);
        SMOT_Motor_stopping = isSMOTStopping();
        break;
    }

    SMOT_Tick();
}

int currentStep = 0;
int substep = 0;

void SMOT_Tick()
{

    //To stop the motor if it has to.

    if (SMOT_ReachedEndPos())
    {
        SMOT_Stop();
    }

    switch (currentStep)
    {
        if(SMOT_Goto(0xFF00, SMOT_SPEED_DEFAULT, X_POSITIVE))
        {

        }
    }
}

uint16_t Bytes_To_Int(uint8_t msb, uint8_t lsb)
{
    return (msb << 8) + lsb;
}

uint16_t SMOT_goingToPos = 0;

void SMOT_ResetCounter()
{
    uint8_t resetCounterPacket[1] = {0x20};
    CAN_TransmitMsg(SMOT_AO_ID, resetCounterPacket, CAN_DLC_1);
}

bool SMOT_Goto(uint16_t pos, uint16_t speed, direction_t dir)
{
    //SMOT_ResetCounter();
    if (!SMOT_Motor_running)
    {
        SMOT_Motor_running = true;

        uint8_t directionSetPacket[1] = {(dir << 4)};
        CAN_TransmitMsg(SMOT_DO_ID, directionSetPacket, CAN_DLC_1);

        uint8_t bytesToSend[3] = {0x20, (speed & 0x00FF), ((speed >> 8) & 0xFF)};
        SMOT_goingToPos = pos;
        CAN_TransmitMsg(SMOT_AO_ID, bytesToSend, CAN_DLC_3);

        return true;
    }
    else
    {
        return false;
    }
}

bool SMOT_Stop()
{
    if (SMOT_Motor_running)
    {
        uint8_t bytesToSend[3] = {0x00, 0x00, 0x00};
        SMOT_goingToPos = 0;
        CAN_TransmitMsg(SMOT_AO_ID, bytesToSend, CAN_DLC_3);
        return true;
    }
    else
    {
        return false;
    }
}

bool SMOT_ReachedEndPos()
{
    if ((current_position >= SMOT_goingToPos) && SMOT_goingToPos != 0)
    {
        return true;
    }
    return false;
}

uint16_t uint8s_To_uint16(uint8_t msb, uint8_t lsb)
{
    uint16_t ret = 0;
    ret |= lsb;
    ret |= msb << 8;

    return ret;
}

//Belt Functions
void SMOT_Move_Delivery_Belt(direction_t dir)
{
    uint8_t bytes_to_send[1];

    switch (dir)
    {
    case X_POSITIVE:
        bytes_to_send[0] = (SMOT_DEL_BELT_POS | do_prev);
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

    switch (dir)
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
