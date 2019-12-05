#include "global.h"

//Testcomment
/***************************************
    Definition of Global Variables
***************************************/
bool ref_slider = false;
bool bero_delivery = false;
bool bero_parked  = false;
bool node_listened = 0;
bool ENT_Ready_To_Give = false;

uint8_t do_prev = 0x00;
bool motorMsgRecieved = false;
uint16_t frequency;

/************
SMOT Variables
************/
bool SMOT_running = false;
bool SMOT_stopping = false;
uint16_t SMOT_current_position = 0;
uint16_t SMOT_goingToPos = 0;
uint16_t SMOT_speed = 0;
direction_t SMOT_direction = X_TO_BELT;
bool SMOT_gotoJobRunning = false;
bool SMOT_shallStopOnRefButton = false;
bool SMOT_overrideLock = false;

/**** Chain *****/
int currentStep = 0;

/***************************************
        Definition of Methods
***************************************/

bool isBERODelivery() { return (Data[0] & SMOT_BERO_DELIVERY); }
bool isBEROParked() { return (Data[0] & SMOT_BERO_PARKED); }
bool isRefSlider() { return (Data[0] & SMOT_REF_DELIVER_SLIDER); }
bool isSMOTStopping() { return (Data[0] & SMOT_AI_STOPPING); }

//Update ONLY function for status variables
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
        SMOT_current_position = uint8s_To_uint16(Data[2], Data[1]);
        SMOT_stopping = isSMOTStopping();

        break;
    }

    SMOT_Tick();
}

//Function for doing control parts.
void SMOT_Tick()
{
    //SMOT_InitSchritkette();
    SMOT_Schrittkette();
}

int smotInitStep = 0;
void SMOT_InitSchritkette()
{
    switch (smotInitStep)
    {
    case 0:
    {
        break;
    }
    case 1:
    {
        SMOT_GoUntilRefButton(SMOT_REF_FROM_TOWER_TO_BELT_LEN, SMOT_SPEED_DEFAULT, X_TO_TOWER, false);

        if (!SMOT_running)
        {
            smotInitStep = 2;
        }
        break;
    }
    case 2:
    {
        if (ref_slider && !SMOT_running)
        {
            smotInitStep = 3;
        }
        else
        {
            //Wait for smot to finish moving

            smotInitStep = 4;
        }
        break;
    }
    case 3:
    {

        SMOT_Goto(SMOT_REF_FROM_BELT_TO_BELT_LEN, SMOT_SPEED_DEFAULT, X_TO_BELT, false);
        if (!SMOT_running)
        {
            smotInitStep = 0;
        }
        break;
    }
    case 4:
    {
        SMOT_GoUntilRefButton(SMOT_MAXPOS, SMOT_SPEED_DEFAULT, X_TO_BELT, true);
        if (!SMOT_running)
        {
            smotInitStep = 5;
        }
        break;
    }
    case 5:
    {

        SMOT_Goto(SMOT_REF_FROM_TOWER_TO_BELT_LEN, SMOT_SPEED_DEFAULT, X_TO_BELT, false);

        if (ref_slider && !SMOT_running)
        {
            smotInitStep = 0;

            //Reset with step 0
            SMOT_InitSchritkette();
        }

        break;
    }
    }
}

int smotStep = 0;
void SMOT_Schrittkette()
{
    switch (smotStep)
    {
    case 0:
    {
        SMOT_goingToPos = 0;
        SMOT_speed = 0;
        SMOT_direction = X_TO_BELT;
        SMOT_shallStopOnRefButton = false;
        SMOT_overrideLock = false;
        SMOT_running = false;
        break;
    }
    case 1:
    {
        SMOT_running = true;
        //Send control packets
        uint8_t directionSetPacket[1] = {SMOT_overrideLock ? (SMOT_UNLOCK_SLIDER | (SMOT_direction << 4)) : (SMOT_direction << 4)};
        CAN_TransmitMsg(SMOT_DO_ID, directionSetPacket, CAN_DLC_1);

        uint8_t bytesToSend[3] = {0x20, (SMOT_speed & 0x00FF), ((SMOT_speed >> 8) & 0xFF)};
        CAN_TransmitMsg(SMOT_AO_ID, bytesToSend, CAN_DLC_3);

        smotStep = 2;
        break;
    }

    case 2:
    {
        if (SMOT_stopping)
        {

        }
        else
        {
           // smotStep = 0;
        }

        smotStep = 3;
        break;
    }
    case 3:
    {
        if (SMOT_current_position >= SMOT_goingToPos || (ref_slider && SMOT_shallStopOnRefButton))
        {
            smotStep = 4;
        }
        break;
    }
    case 4:
    {
        SMOT_Stop();
        smotStep = 5;
        break;
    }
    case 5:
    {
        if (SMOT_stopping)
        {
            smotStep = 6;
        }
        break;
    }
    case 6:
    {
        if (SMOT_stopping != true)
        {
            smotStep = 0;

            //Reset with step 0
            SMOT_Schrittkette();
        }
        break;
    }
    }
}

void SMOT_Init()
{
    CAN_NMTConnect();

   // smotInitStep = 1;

   // SMOT_Tick();
}

bool SMOT_Goto(uint16_t pos, uint16_t speed, direction_t dir, bool overrideLock)
{
    if (smotStep != 0)
        return false;

    SMOT_goingToPos = pos;
    SMOT_speed = speed;
    SMOT_direction = dir;
    SMOT_overrideLock = overrideLock;

    //START SCHRITTKETTE
    smotStep = 1;

    SMOT_Tick();

    return true;
}

bool SMOT_GoUntilRefButton(uint16_t pos, uint16_t speed, direction_t dir, bool overrideLock)
{
    if (smotStep != 0)
        return false;

    SMOT_shallStopOnRefButton = true;
    return SMOT_Goto(pos, speed, dir, overrideLock);
}

void SMOT_Stop()
{
    //Send control packet
    uint8_t bytesToSend[3] = {0x00, 0x00, 0x00};
    CAN_TransmitMsg(SMOT_AO_ID, bytesToSend, CAN_DLC_3);
}

bool SMOT_Running(){
    return SMOT_running;
}

uint16_t uint8s_To_uint16(uint8_t msb, uint8_t lsb)
{
    return (uint16_t)((msb << 8) | lsb);
}

//Belt Functions
void SMOT_Move_Delivery_Belt(direction_t dir)
{
    uint8_t bytes_to_send[1];

    switch (dir)
    {
    case X_TO_TOWER:
        bytes_to_send[0] = (SMOT_DEL_BELT_POS | do_prev);
        break;

    case X_TO_BELT:
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
    case X_TO_TOWER:
        bytes_to_send[0] = SMOT_DEL_BELT_POS;
        break;

    case X_TO_BELT:
        bytes_to_send[0] = SMOT_DEL_BELT_NEG;
        break;

    default:
        break;
    }

    CAN_TransmitMsg(SMOT_DO_ID, bytes_to_send, CAN_DLC_1);
}
