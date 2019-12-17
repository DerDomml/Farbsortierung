/****************************************************
	Farbsortierungs Includes
****************************************************/
#ifndef FS_FARBSORTIERUNG_H
#define FS_FARBSORTIERUNG_H

#define FS_SORT_INFILTER        0x00
#define FS_SORT_OUTFILTER       0x00
#define FS_SORT_NODE_ID         0x12
#define FS_SORT_DI_11Bit_ID     (0x180 + FS_SORT_NODE_ID)
#define FS_SORT_DO_11Bit_ID     (0x200 + FS_SORT_NODE_ID)
//InPut
#define FS_SORT_NotAus          0x40

#define FS_SORT_inMag3          0x20
#define FS_SORT_inMag2          0x10
#define FS_SORT_inMag1          0x08

#define FS_SORT_SensorAusgang3  0x04
#define FS_SORT_SensorAusgang2  0x02
#define FS_SORT_SensorAusgang1  0x01
//OutPut
#define FS_SORT_StangeUntenAuf  0x80
#define FS_SORT_StangeUntenZu   0x40
#define FS_SORT_StangeObenAuf   0x20
#define FS_SORT_StangeObenZu    0x10

#define FS_SORT_LampeSensor     0x08


#define FS_SORT_DataLength      1

#define FS_SORT_ColorOrange     1
#define FS_SORT_ColorBlue       2
#define FS_SORT_ColorGreen      3

#define FS_SORT_MaxOrders       3

#define FS_SORT_MaxBallsinMag   11

#define FS_SORT_UartSendAcknowledge 1

struct Order {
    uint8_t AmountOfOrderedOrangeBalls;
    uint8_t AmountOfOrderedBlueBalls;
    uint8_t AmountOfOrderedGreenBalls;
};

bool FS_SORT_isOrder;

void FS_SORT_NewData(CanRxMsg RxMessage);
void FS_SORT_Routine();
void FS_SORT_CloseStangen();
uint8_t FS_SORT_GetColor();

uint8_t FS_SORT_GetAmount(uint8_t Color);
void FS_SORT_AddBall(uint8_t Color);
void FS_SORT_RemoveBall(uint8_t Color);
void FS_SORT_SetStartAmount(uint16_t RxData);
void FS_SORT_GetOrder(uint16_t RxData);

void FS_SORT_ActivateLight();
void FS_SORT_DeactivateLight();

bool FS_SORT_GetCurrentAmountOfOrderedBalls(uint8_t* ballsOrange, uint8_t* ballsBlue, uint8_t* ballsGreen);
bool FS_SORT_FirstOrderEmpty();
void FS_SORT_UpdateArray();
void FS_SORT_SendToPc();

#endif
