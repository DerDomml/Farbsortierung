#ifndef CAN_H__
#define CAN_H__

#define CAN_NMTID       0x00
#define CAN_ActorID     0x201
#define CAN_SensorID    0x181

#define CAN_NMTStart    0x01
#define CAN_NMTAllNodes 0x00

#define CAN_DLC_0       0
#define CAN_DLC_1       1
#define CAN_DLC_2       2
#define CAN_DLC_3       3
#define CAN_DLC_4       4
#define CAN_DLC_5       5
#define CAN_DLC_6       6
#define CAN_DLC_7       7
#define CAN_DLC_8       8

/*********************************************************************************


*********************************************************************************/

void CAN_Init4Models();

uint8_t CAN_TransmitMsg(uint16_t canId, uint8_t *canData, uint8_t canDlc);

uint8_t CAN_NMTConnect();

uint8_t Msg_ID;

char Data[8];

#endif // !CAN_H__
