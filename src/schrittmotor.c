#include "global.h"

//Dommi Kommi

//Kommi B�rni

void motorInit(){
    uint8_t d[] = {2,0,0,0,0,0,0,0};
    CAN_TransmitMsg(0x215, d, CAN_DLC_1);
}
