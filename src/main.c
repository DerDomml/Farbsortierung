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
    }
}
