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

    FS_ENTLEER_CAN_received = false;

    while(TRUE)
    {
       if(FS_ENTLEER_CAN_received) {
            FS_ENTLEER_Tick();
       }






      //  uint8_t notaus[2] = {0x00, 0x00};


        //CAN_TransmitMsg(0x213, notaus, 2);
    }
}
