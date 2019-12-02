/*******************************
    Includes für Schrittmotor
*******************************/

#ifndef SCHRITTMOTOR_H

#define SMOT_NODE_ID 0x15               //Node-ID
#define SMOT_DI_BYTE 0x180              //Digital Input Byte
#define SMOT_AI_BYTES 0x380             //Analog Input Byte, 0 Status Byte, 1 K1_LB, 2K1_HB
#define SMOT_DO_BYTE 0x200              //Digital Output Byte
#define SMOT_AO_BYTES 0x400             //Analog Output Byte 0 Control Byte, 1 K1_LB, 2 K1_HB


//Digital Inputs
#define SMOT_BERO_Parking 0x01
#define SMOT_BERO_Delivering 0x02
#define SMOT_Ref_Deliver_Slider 0x04

//Analog Inputs

#define SCHRITTMOTOR_H
#endif


void MotorInit();
