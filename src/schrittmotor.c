#include "global.h"

//Testcomment
/***************************************
    Definition of Global Variables
***************************************/
bool ref_slider = false;
bool bero_delivery = false;
bool bero_parked = false;
bool node_listened = 0;
bool ENT_Ready_To_Give = false;
bool FS_KOM_Ready_To_Take = false;
bool FS_KOM_Taking_Complete = false;

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
int smotInitStep = 0;
int smotStep = 0;

direction_t SMOT_direction = X_TO_BELT;
bool SMOT_gotoJobRunning = false;
bool SMOT_shallStopOnRefButton = false;
bool SMOT_overrideLock = false;
bool SMOT_InitComplete = false;
bool SMOT_Ready_To_Give = false;
bool SMOT_Ready_To_Take = false;
bool SMOT_Taking_Complete = false;

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
    SMOT_Schrittkette();
    SMOT_Transport_Block();
    SMOT_InitSchritkette();
}

int SMOT_transportStep = 0;

void SMOT_Transport_Block()
{
    switch(SMOT_transportStep)
    {
    case 0:
    {
        if(SMOT_InitComplete)
        {
            SMOT_transportStep = 1;
        }
    }
    case 1:
    {
        if(bero_parked && !bero_delivery)
        {
            SMOT_Start_Parking_Belt(X_TO_SYSTEM);
            SMOT_Start_Delivery_Belt(X_TO_SYSTEM);
        }

        if(bero_delivery)
        {
            SMOT_Ready_To_Give = true;
            SMOT_transportStep = 2;
        }

        break;
    }
    case 2:
    {
        if(FS_KOM_Ready_To_Take)
        {
            SMOT_Start_Delivery_Belt(X_TO_SYSTEM);
            for(int i = 0; i < 200000; i++);
            for(int i = 0; i < 200000; i++);
            for(int i = 0; i < 200000; i++);
            for(int i = 0; i < 200000; i++);
            for(int i = 0; i < 200000; i++);
            FS_KOM_Taking_Complete = true;
            SMOT_transportStep = 3;
        }
    }
    case 3:
    {
        if(FS_KOM_Taking_Complete)
        {
            SMOT_Stop_Delivery_Belt();
            SMOT_transportStep = 4;
        }
    }
    case 4:
    {
        SMOT_GoUntilRefButton(SMOT_MAXPOS, SMOT_SPEED_DEFAULT, X_TO_TOWER, false);

        if(ref_slider)
        {
            SMOT_transportStep = 5;
        }
    }
    case 5:
    {
        SMOT_Goto(SMOT_TOWER_POS, SMOT_SPEED_DEFAULT, X_TO_TOWER, false);

        SMOT_transportStep = 6;
    }
    case 6:
    {
        if(!SMOT_Running())
        {
            SMOT_transportStep = 7;
        }
    }
    case 7:
    {
        SMOT_Ready_To_Take = true;

        if(ENT_Ready_To_Give)
        {
            SMOT_Start_Delivery_Belt(X_TO_PARKING);
            SMOT_transportStep = 8;
        }
    }
    case 8:
    {
        if(bero_delivery)
        {
            SMOT_Taking_Complete = true;
            SMOT_transportStep = 9;
        }
    }
    case 9:
    {
        SMOT_GoUntilRefButton(SMOT_MAXPOS, SMOT_SPEED_DEFAULT, X_TO_BELT, false);

        if(ref_slider)
        {
            SMOT_transportStep = 10;
        }
    }
    case 10:
    {
        SMOT_Goto(SMOT_REF_FROM_TOWER_TO_BELT_LEN, SMOT_SPEED_DEFAULT, X_TO_BELT, false);

        SMOT_transportStep = 11;
    }
    case 11:
    {
        if(!SMOT_Running())
        {
            SMOT_transportStep = 1;
        }
    }


    }
}

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
        smotInitStep = 2;
        break;
    }
    case 2:
    {
        if (ref_slider)
        {
            smotInitStep = 3;
        }
        else if (!SMOT_running)
        {
            //Wait for smot to finish moving

            smotInitStep = 6;
        }

        break;
    }
    case 3:
    {
        if (!SMOT_running)
        {
            smotInitStep = 4;
        }
        break;
    }
    case 4:
    {
        SMOT_Goto(SMOT_REF_FROM_BELT_TO_BELT_LEN, SMOT_SPEED_DEFAULT, X_TO_BELT, false);
        smotInitStep = 5;
        break;
    }
    case 5:
    {

        if (!SMOT_running)
        {
            smotInitStep = 0;
        }
        break;
    }
    case 6:
    {
        SMOT_GoUntilRefButton(SMOT_MAXPOS, SMOT_SPEED_DEFAULT, X_TO_BELT, true);
        smotInitStep = 7;
    }
    case 7:
    {
        if (!SMOT_running)
        {
            smotInitStep = 8;
        }
        break;
    }
    case 8:
    {

        SMOT_Goto(SMOT_REF_FROM_TOWER_TO_BELT_LEN, SMOT_SPEED_DEFAULT, X_TO_BELT, false);

        smotInitStep = 9;

        break;
    }
    case 9:
    {

        if (ref_slider && !SMOT_running)
        {
            smotInitStep = 0;
            SMOT_InitComplete = true;

            //Reset with step 0
            SMOT_InitSchritkette();
        }
    }
    }
}

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
        SMOT_current_position = 0;
        break;
    }
    case 1:
    {
        //Send control packets
        uint8_t directionSetPacket[1] = {SMOT_overrideLock ? (SMOT_UNLOCK_SLIDER | (SMOT_direction << 4)) : (SMOT_direction << 4)};
        if (CAN_TransmitMsg(SMOT_DO_ID, directionSetPacket, CAN_DLC_1) <= 2)
        {

            smotStep = 2;
        }

        break;
    }

    case 2:
    {
        uint8_t bytesToSend[3] = {0x20, (SMOT_speed & 0x00FF), ((SMOT_speed >> 8) & 0xFF)};
        if (CAN_TransmitMsg(SMOT_AO_ID, bytesToSend, CAN_DLC_3) <= 2)
        {

            smotStep = 3;
        }
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
            //SMOT_stopping = false;
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

    smotInitStep = 1;

    //SMOT_Tick();
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
    SMOT_running = true;

    //SMOT_Tick();

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

bool SMOT_Running()
{
    return SMOT_running;
}

uint16_t uint8s_To_uint16(uint8_t msb, uint8_t lsb)
{
    return (uint16_t)((msb << 8) | lsb);
}

//Belt Functions
void SMOT_Start_Delivery_Belt(belt_direction_t dir)
{
    uint8_t _bytes_to_send[1];

    switch (dir)
    {
    case X_TO_SYSTEM:
        _bytes_to_send[0] = (SMOT_DEL_BELT_POS | do_prev);
        break;

    case X_TO_PARKING:
        _bytes_to_send[0] = (SMOT_DEL_BELT_NEG | do_prev);
        break;

    default:
        break;
    }

    do_prev |= _bytes_to_send[0];
    CAN_TransmitMsg(SMOT_DO_ID, _bytes_to_send, CAN_DLC_1);
}

void SMOT_Stop_Delivery_Belt()
{
    uint8_t _bytes_to_send[1];

    _bytes_to_send[0] = (do_prev & !(SMOT_DEL_BELT_POS | SMOT_DEL_BELT_NEG));

    CAN_TransmitMsg(SMOT_DO_ID, _bytes_to_send, CAN_DLC_1);

    do_prev |= _bytes_to_send[0];
}

void SMOT_Start_Parking_Belt(belt_direction_t dir)
{
    uint8_t _bytes_to_send[1];

    switch (dir)
    {
    case X_TO_SYSTEM:
        _bytes_to_send[0] = SMOT_DEL_BELT_POS;
        break;

    case X_TO_PARKING:
        _bytes_to_send[0] = SMOT_DEL_BELT_NEG;
        break;

    default:
        break;
    }

    CAN_TransmitMsg(SMOT_DO_ID, _bytes_to_send, CAN_DLC_1);

    do_prev |= _bytes_to_send[0];
}

void SMOT_Stop_Parking_Belt()
{
    uint8_t _bytes_to_send[1];

    _bytes_to_send[0] = (do_prev & !(SMOT_PAR_BELT_POS | SMOT_PAR_BELT_NEG));

    CAN_TransmitMsg(SMOT_DO_ID, _bytes_to_send, CAN_DLC_1);

    do_prev |= _bytes_to_send[0];
}

