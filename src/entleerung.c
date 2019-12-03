#include "global.h"

void FS_ENTLEER_Tick() {

    uint16_t CAN_received = Data;
    uint16_t CAN_toSend = 0;
    FS_ENTLEER_CAN_received = false;



    if(CAN_received & 0x0800) {
        CAN_toSend |= 0x8000;
    }

}
