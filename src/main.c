/*
**                           Main.c
**
**********************************************************************/
/*
    Manual:


**********************************************************************/
#include "global.h"

int main(void)
{
    ProjectInit();
    CAN_NMTConnect();

    FS_ENTLEER_CAN_NewTelegramReceived = false;
    FS_ENTLEER_GREIFARM_Tick = false;

    while(TRUE)
    {
       if(FS_ENTLEER_CAN_NewTelegramReceived || FS_ENTLEER_GREIFARM_Tick){
            FS_ENTLEER_Tick();
        }
//        uint8_t Greifarmhoch[2] = {0x02, 0x00};
//        uint8_t AnTurmHoch[2] = {0x80, 0x00};
//        uint8_t notaus[2] = {0x00, 0x00};
//
//        CAN_TransmitMsg(0x213, Greifarmhoch, 2);
//        CAN_TransmitMsg(0x213, notaus, 2);
//        CAN_TransmitMsg(0x213, AnTurmHoch, 2);
//        CAN_TransmitMsg(0x213, notaus, 2);
    }
}
