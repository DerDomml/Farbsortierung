#include "global.h"

static uint8_t FS_SORT_InData[FS_SORT_DataLength];

static uint8_t FS_SORT_OutData[FS_SORT_DataLength];

static uint8_t FS_SORT_Color;

// Amount of Balls left in the mags
static uint8_t FS_SORT_AmountOfOrangeBalls = 1;
static uint8_t FS_SORT_AmountOfBlueBalls = 1;
static uint8_t FS_SORT_AmountOfGreenBalls = 1;

static struct Order FS_SORT_OrderArray[FS_SORT_MaxOrders];
static uint8_t FS_SORT_CurrentAmountOfOrders = 0;

/*******************************************************************
*																   *
*		 Aktualisierung der Aktoren     						   *
*																   *
********************************************************************/

bool FS_SORT_IsInFrontOfMag1() {return (FS_SORT_InData[0] & FS_SORT_inMag1) ? true : false;}
bool FS_SORT_IsInFrontOfMag2() {return (FS_SORT_InData[0] & FS_SORT_inMag2) ? true : false;}
bool FS_SORT_IsInFrontOfMag3() {return (FS_SORT_InData[0] & FS_SORT_inMag3) ? true : false;}

bool FS_SORT_ColorIsOrange() {return (FS_SORT_InData[0] & FS_SORT_SensorAusgang1) ? true : false;}
bool FS_SORT_ColorIsBlue() {return (FS_SORT_InData[0] & FS_SORT_SensorAusgang2) ? true : false;}
bool FS_SORT_ColorIsGreen() {return (FS_SORT_InData[0] & FS_SORT_SensorAusgang3) ? true : false;}

// Receive of new data
void FS_SORT_NewData(CanRxMsg RxMessage) {
    if(RxMessage.DLC == CAN_DLC_1)
        FS_SORT_InData[0] = RxMessage.Data[0];

    FS_SORT_Routine();
}

void FS_SORT_Routine() {
    if(FS_SORT_Color == 0)
        FS_SORT_Color = FS_SORT_GetColor();

    // Orange ball detected
    if(FS_SORT_IsInFrontOfMag1() && FS_SORT_Color == FS_SORT_ColorOrange) {
        FS_SORT_OutData[0] = FS_SORT_StangeObenAuf | FS_SORT_StangeUntenAuf;
        FS_SORT_Color = 0;
        FS_SORT_AddBall(FS_SORT_ColorOrange);
        TIM_Cmd(TIM3, ENABLE);
    }

    // Blue ball detected
    if(FS_SORT_IsInFrontOfMag2() && FS_SORT_Color == FS_SORT_ColorBlue) {
        FS_SORT_OutData[0] = FS_SORT_StangeObenAuf | FS_SORT_StangeUntenAuf;
        FS_SORT_Color = 0;
        FS_SORT_AddBall(FS_SORT_ColorBlue);
        TIM_Cmd(TIM3, ENABLE);
    }

    // Green ball detected
    if(FS_SORT_IsInFrontOfMag3() && FS_SORT_Color == FS_SORT_ColorGreen) {
        FS_SORT_OutData[0] = FS_SORT_StangeObenAuf | FS_SORT_StangeUntenAuf;
        FS_SORT_Color = 0;
        FS_SORT_AddBall(FS_SORT_ColorGreen);
        TIM_Cmd(TIM3, ENABLE);
    }
    //TODO Update nur wenn nötig
    CAN_TransmitMsg(FS_SORT_DO_11Bit_ID, FS_SORT_OutData, FS_SORT_DataLength);
}

void FS_SORT_CloseStangen() {
    FS_SORT_OutData[0] = FS_SORT_StangeObenZu | FS_SORT_StangeUntenZu;
    CAN_TransmitMsg(FS_SORT_DO_11Bit_ID, FS_SORT_OutData, FS_SORT_DataLength);
    TIM_Cmd(TIM3, DISABLE);
}

uint8_t FS_SORT_GetColor() {
    if(FS_SORT_ColorIsOrange())
        return FS_SORT_ColorOrange;
    if(FS_SORT_ColorIsBlue())
        return FS_SORT_ColorBlue;
    if(FS_SORT_ColorIsGreen())
        return FS_SORT_ColorGreen;
    return 0;
}

// Get amount of balls in the mag
uint8_t FS_SORT_GetAmount(uint8_t Color) {
    if(Color == FS_SORT_ColorOrange)
        return FS_SORT_AmountOfOrangeBalls;
    if(Color == FS_SORT_ColorBlue)
        return FS_SORT_AmountOfBlueBalls;
    if(Color == FS_SORT_ColorGreen)
        return FS_SORT_AmountOfGreenBalls;
    return 0;
}

// Set amount of balls in the mag
void FS_SORT_AddBall(uint8_t Color){
    switch (Color)
    {
        case FS_SORT_ColorOrange: FS_SORT_AmountOfOrangeBalls++; break;
        case FS_SORT_ColorBlue: FS_SORT_AmountOfBlueBalls++; break;
        case FS_SORT_ColorGreen: FS_SORT_AmountOfGreenBalls++; break;
        default: break;
    }
}

void FS_SORT_RemoveBall(uint8_t Color){
    switch (Color)
    {
        case FS_SORT_ColorOrange: FS_SORT_AmountOfOrangeBalls--; break;
        case FS_SORT_ColorBlue: FS_SORT_AmountOfBlueBalls--; break;
        case FS_SORT_ColorGreen: FS_SORT_AmountOfGreenBalls--; break;
    }
}

void FS_SORT_SetStartAmount(uint16_t RxData) {
    int amount;
    int color;

    color = RxData % 10;
    amount = RxData - color;
    switch(color){
        case FS_SORT_ColorOrange: FS_SORT_AmountOfOrangeBalls = amount/10; break;
        case FS_SORT_ColorBlue:FS_SORT_AmountOfBlueBalls = amount/10; break;
        case FS_SORT_ColorGreen:FS_SORT_AmountOfGreenBalls = amount/10; FS_SORT_isOrder = true; break;
    }
}

void FS_SORT_GetOrder(uint16_t RxData) {
    if(FS_SORT_CurrentAmountOfOrders < FS_SORT_MaxOrders) {

        int amount;
        int color;

        color = RxData % 10;
        amount = RxData - color;
        switch(color){
            case FS_SORT_ColorOrange: {
                FS_SORT_OrderArray[FS_SORT_CurrentAmountOfOrders].AmountOfOrderedOrangeBalls = amount/10;
                break;
            }
            case FS_SORT_ColorBlue: {
                FS_SORT_OrderArray[FS_SORT_CurrentAmountOfOrders].AmountOfOrderedBlueBalls  = amount/10;
                break;
            }
            case FS_SORT_ColorGreen: {
                FS_SORT_OrderArray[FS_SORT_CurrentAmountOfOrders].AmountOfOrderedGreenBalls  = amount/10;
                FS_SORT_CurrentAmountOfOrders++;
                break;
            }
        }
    }
}

void FS_SORT_ActivateLight() {
    FS_SORT_OutData[0] |= FS_SORT_LampeSensor;
    CAN_TransmitMsg(FS_SORT_DO_11Bit_ID, FS_SORT_OutData, FS_SORT_DataLength);
}

void FS_SORT_DeactivateLight() {
    FS_SORT_OutData[0] &= ~FS_SORT_LampeSensor;
    CAN_TransmitMsg(FS_SORT_DO_11Bit_ID, FS_SORT_OutData, FS_SORT_DataLength);
}

bool FS_SORT_GetCurrentAmountOfOrderedBalls(uint8_t* ballsOrange, uint8_t* ballsBlue, uint8_t* ballsGreen) {
    if(!FS_SORT_FirstOrderEmpty()) {
       *ballsOrange = FS_SORT_OrderArray[0].AmountOfOrderedOrangeBalls;
       *ballsBlue = FS_SORT_OrderArray[0].AmountOfOrderedBlueBalls;
       *ballsGreen = FS_SORT_OrderArray[0].AmountOfOrderedGreenBalls;

       return true;
    }
    else
    {
        return false;
    }
}

bool FS_SORT_FirstOrderEmpty() {
    if(FS_SORT_OrderArray[0].AmountOfOrderedOrangeBalls == 0)
        if(FS_SORT_OrderArray[0].AmountOfOrderedBlueBalls == 0)
            if(FS_SORT_OrderArray[0].AmountOfOrderedGreenBalls == 0)
                return true;
    return false;
}

void FS_SORT_UpdateArray() {
    FS_SORT_OrderArray[0] = FS_SORT_OrderArray[1];
    FS_SORT_OrderArray[1] = FS_SORT_OrderArray[2];

    FS_SORT_OrderArray[2].AmountOfOrderedOrangeBalls = 0;
    FS_SORT_OrderArray[2].AmountOfOrderedBlueBalls = 0;
    FS_SORT_OrderArray[2].AmountOfOrderedGreenBalls = 0;
    FS_SORT_CurrentAmountOfOrders--;
    FS_SORT_SendToPc();
}

void FS_SORT_SendToPc() {
    uint8_t UartAcknowledge = 0x01;
    USART_SendData(USART1, UartAcknowledge);
}
