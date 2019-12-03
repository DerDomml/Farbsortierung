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

    while(TRUE)
    {
        //if(FS_ENTLEER_CAN_received) {
        //    FS_ENTLEER_Tick();
        //}
    uint8_t data[] = {0x10, 0x00};
    CAN_TransmitMsg(0x213, data, 2);

    uint8_t data2[] = {0x00, 0x00};
    CAN_TransmitMsg(0x213, data2, 2);
    }
}
