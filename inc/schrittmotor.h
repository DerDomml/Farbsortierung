/*******************************
    Includes f�r Schrittmotor
*******************************/

#ifndef SCHRITTMOTOR_H
#define SCHRITTMOTOR_H

#define SMOT_NODE_ID 0x15               //Node-ID
#define SMOT_DI_ID 0x195                //Digital Input Byte
#define SMOT_AI_ID 0x395                //Analog Input Byte, 0 Status Byte, 1 K1_LB, 2K1_HB
#define SMOT_DO_ID 0x215                //Digital Output Byte
#define SMOT_AO_ID 0x415                //Analog Output Byte 0 Control Byte, 1 K1_LB, 2 K1_HB
#define SMOT_STATUS_ID 0x715


//Digital Inputs
#define SMOT_BERO_PARKED 0x01
#define SMOT_BERO_DELIVERY 0x02
#define SMOT_REF_DELIVER_SLIDER 0x04

//Analog Inputs
#define SMOT_SB 0xFF0000                //Control Byte
#define SMOT_AI_K1_LB 0x00FF00          //Input K1 Low Byte
#define SMOT_AI_K1_HB 0x0000FF          //Input K1 High Byte

//Digital Outputs
#define SMOT_UNLOCK_SLIDER 0x20
#define SMOT_INVERT_MOTOR 0x10
#define SMOT_DEL_BELT_NEG 0x08
#define SMOT_DEL_BELT_POS 0x04
#define SMOT_PAR_BELT_NEG 0x02
#define SMOT_PAR_BELT_POS 0x01

//Analog Outputs
#define SMOT_CB 0xFF0000                //Control Byte
#define SMOT_CB_FREQ 0x01               //Base Frequency
#define SMOT_CB_RAMP_OFF 0x02
#define SMOT_CB_DRIVE_TO_VALUE 0x04
#define SMOT_CB_CLEAR_COUNTER 0x20
#define SMOT_CB_TRANSMIT_DATA 0x80
#define SMOT_AO_K1_LB 0x00FF00          //Output K1 Low Byte
#define SMOT_AO_K1_HB 0x0000FF          //Output K2 High Byte

//Step Constants
#define SMOT_ENDPOS 0x20B7              //Steps where slider reaches end position
#define SMOT_SPEED_DEFAULT 0x012C       //Process Data for default speed

typedef enum direction
{
    X_NEGATIVE = 0,
    X_POSITIVE
}
direction_t;

//SMOT Methods Declaration
void SMOT_Init();
void SMOT_Update();
void SMOT_Tick();
void SMOT_Motor_Start();
uint16_t uint8s_To_uint16(uint8_t msb,uint8_t lsb);
bool SMOT_ReachedEndPos();
void SMOT_Stop();
void SMOT_Goto(uint16_t pos, uint16_t speed, direction_t dir);

uint16_t Bytes_To_Int(uint8_t toSwap1, uint8_t toSwap2);

#endif
