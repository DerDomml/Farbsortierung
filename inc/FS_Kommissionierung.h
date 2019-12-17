/****************************************************
	Kommissionierungs Defines
****************************************************/
#ifndef FS_KOMMISSIONIERUNG_H
#define FS_KOMMISSIONIERUNG_H

#define FS_KOM_INFILTER     0x00
#define FS_KOM_OUTFILTER    0x00

#define FS_KOM_NODE_ID      0x11    //Node ID 17

#define FS_KOM_DI_11Bit_ID  (0x180+FS_KOM_NODE_ID)   //0x180(+node ID)
#define FS_KOM_DO_11Bit_ID  (0x200+FS_KOM_NODE_ID)   //0x200(+node ID)

#define FS_KOM_DI_LT1       0x40    //Byte 0
#define FS_KOM_DI_LT2       0x80    //Byte 0
#define FS_KOM_DI_LT3       0x01    //Byte 1

#define FS_KOM_DI_BERO1     0x08    //Byte 0
#define FS_KOM_DI_BERO2     0x10    //Byte 0
#define FS_KOM_DI_BERO3     0x20    //Byte 0
#define FS_KOM_DI_BERO0     0x01    //Byte 0

#define FS_KOM_DI_REED1     0x02    //Byte 0
#define FS_KOM_DI_REED2     0x04    //Byte 0

#define FS_KOM_DO_KuFrMag3  0x01    //Byte 0
#define FS_KOM_DO_KuFrMag2  0x04    //Byte 0
#define FS_KOM_DO_KuFrMag1  0x10    //Byte 0

#define FS_KOM_DO_VerMag3   0x80    //Byte 1
#define FS_KOM_DO_VerMag2   0x02    //Byte 0
#define FS_KOM_DO_VerMag1   0x08    //Byte 0

#define FS_KOM_DO_BAND_Xplus   0x40    //Byte 0
#define FS_KOM_DO_BAND_Xminus   0x20    //Byte 0
#define FS_KOM_DO_IBAND_Xplus  0x80    //Byte 0

#define FS_KOM_POS0         0x01
#define FS_KOM_POS1         0x02
#define FS_KOM_POS2         0x03
#define FS_KOM_POS3         0x04

#define FS_KOM_DATALength   2

//Program State Definition
enum {
    FS_KOM_Step_AddBlock        = 1,
    FS_KOM_Step_MoveToBero1     = 2,
    FS_KOM_Step_FillColor1      = 3,
    FS_KOM_Step_MoveToBero2     = 4,
    FS_KOM_Step_FillColor2      = 5,
    FS_KOM_Step_MoveToBero3     = 6,
    FS_KOM_Step_FillColor3      = 7,
    FS_KOM_Step_WaitForServo    = 8,
    FS_KOM_Step_MoveToServo     = 9,
    FS_KOM_Step_ResetVerMag     = 10
};

//Global Informations
uint8_t FS_KOM_Timer;
uint8_t FS_KOM_Schritt;

//Global States
bool FS_KOM_NEWTelegram;
bool FS_KOM_Block_Waiting;

//Connection States
bool FS_Vorgaenger;//-----------------------------------------------Vorgaenger!!!!
bool FS_KOM_Ready_to_take_Block;
bool FS_KOM_Ready_to_give_Block;

//Initialize "Kommissionierung"
void FS_KOM_Init();

//Receive new Data
void FS_KOM_NewData(CanRxMsg RxMessage);

//Routine for "Kommissionierung"
void FS_KOM_Routine();

//Set Step and check Transitions
uint8_t FS_KOM_Schrittwahl(uint8_t *Farbe1, uint8_t *Farbe2, uint8_t *Farbe3);

//Write Data According to Step
void FS_KOM_WRITE_Data(uint8_t *FS_KOM_DO_OUTData,uint8_t *FS_KOM_Schritt,uint8_t *Farbe1, uint8_t *Farbe2, uint8_t *Farbe3);

//Generate Number of Colors by Reed Input
void FS_KOM_Auftrag(uint8_t Zahl, uint8_t *Farbe1,uint8_t *Farbe2,uint8_t *Farbe3);

//Check if Routine should be Accessed
bool FS_KOM_CHECK_RoutineCondition();

void FS_KOM_OrderBalls();

//Check Sensors
bool FS_KOM_CHECK_DI_LT1();
bool FS_KOM_CHECK_DI_LT2();
bool FS_KOM_CHECK_DI_LT3();
bool FS_KOM_CHECK_DI_BERO1();
bool FS_KOM_CHECK_DI_BERO2();
bool FS_KOM_CHECK_DI_BERO3();
bool FS_KOM_CHECK_DI_BERO0();
bool FS_KOM_CHECK_DI_REED1();
bool FS_KOM_CHECK_DI_REED2();

//Activate Actors
void FS_KOM_ENABLE_DO_KuFrMag3(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_KuFrMag3(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_ENABLE_DO_KuFrMag2(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_KuFrMag2(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_ENABLE_DO_KuFrMag1(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_KuFrMag1(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_ENABLE_DO_VerMag3(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_VerMag3(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_ENABLE_DO_VerMag2(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_VerMag2(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_ENABLE_DO_VerMag1(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_VerMag1(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_ENABLE_DO_BAND_Xplus(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_BAND_Xplus(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_ENABLE_DO_BAND_Xminus(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_BAND_Xminus(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_ENABLE_DO_IBAND_Xplus(uint8_t *FS_KOM_DO_OUTData);
void FS_KOM_DISABLE_DO_IBAND_Xplus(uint8_t *FS_KOM_DO_OUTData);

#endif
