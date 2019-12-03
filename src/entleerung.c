#include "global.h"


uint8_t CAN_toSend_Array[2] = {0, 0};
uint16_t CAN_toSend = 0;


bool isEntleerPos(){ return (CAN_received & FS_ENTLEER_ENTLEERPOS_SENSOR)   ? true : false;  }
bool isBOEndePos(){ return (CAN_received & FS_ENTLEER_BO_ENDE_SENSOR)       ? true : false;  }

void FS_ENTLEER_Tick() {

    CAN_received = Data;

    FS_ENTLEER_CAN_received = false;





    if(isEntleerPos()) {
        CAN_toSend |= FS_ENTLEER_BANDLAUF_OBEN;
    }

    if(isBOEndePos()){
        CAN_toSend &= ~FS_ENTLEER_BANDLAUF_OBEN;
    }


    CAN_toSend_Array[1] = CAN_toSend;
    CAN_toSend_Array[0] = CAN_toSend >> 8;
    CAN_TransmitMsg(FS_ENTLEER_CAN_SEND_ID, CAN_toSend_Array, CAN_DLC_2);
}
