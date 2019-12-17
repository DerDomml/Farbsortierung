/****************************************************
	ServoMotor Includes
****************************************************/
#ifndef FS_SERVOMOTOR_H
#define FS_SERVOMOTOR_H

#define FS_SERVO_DLC_IN             1
#define FS_SERVO_DI_11Bit_ID     0x194
#define FS_SERVO_DO_11Bit_ID     0x214
#define FS_SERVO_AO_11Bit_ID     0x314

//Defines for conveyor
#define FS_SERVO_CONVEYOR_STOP      0x00
#define FS_SERVO_CONVEYOR_RIGHT     0x01
#define FS_SERVO_CONVEYOR_LEFT      0x02

//Defines Masks for digital inputs
#define FS_SERVO_BERO_MASK          0x01
#define FS_SERVO_REF_MASK           0x02
#define FS_SERVO_INC_A_MASK         0x04
#define FS_SERVO_INC_B_MASK         0x08

//Defines for motor steps
#define FS_SERVO_STEPS_TO_REF       13
#define FS_SERVO_STEPS_TO_EMPTYING  341

//Defines for timer
#define FS_SERVO_TIMER_ARR          30000

//Functions
void FS_Servo_init();
void FS_SERVO_Routine();
void FS_SERVO_NewData(CanRxMsg RxMessage);
void FS_SERVO_controlMotor(uint8_t direction);
void FS_SERVO_conveyorMovement(uint8_t direction);

bool FS_SERVO_BeroActive();
bool FS_SERVO_IsRefPressed();
bool FS_SERVO_IncAChanged();

//Variables
bool FS_SERVO_arrived;
bool FS_SERVO_setTimer;
bool FS_SERVO_timerDone;
bool FS_SERVO_blockReady;

int16_t counterA;

uint8_t step;
uint8_t direction;
uint8_t prevDirection;
uint8_t conveyorDirection;
uint8_t conveyorDirectionPrev;
uint8_t FS_SERVO_InData[FS_SERVO_DLC_IN];

enum Sequencer{
    FS_SERVO_INITIAL_MOVE = 0,
    FS_SERVO_DRIVE_TO_REFKEY = 1,
    FS_SERVO_DRIVE_TO_PICKING = 2,
    FS_SERVO_WAITING_FOR_BLOCK = 3,
    FS_SERVO_TAKE_BLOCK = 4,
    FS_SERVO_DRIVE_TO_EMPTYING = 5,
    FS_SERVO_STOP_AT_EMPTYING = 6,
    FS_SERVO_WAIT_FOR_EMPTYING = 7,
    FS_SERVO_STOP_CONVEYOR = 8,
};

enum DirectionState{
    MotorStop = 0,
    MoveToPicking = 1,
    MoveToEmptying = 2,
};

enum ConveyorState{
    conveyorStop = 0,
    conveyorLeft = 1,
    conveyorRight = 2,
};

#endif
