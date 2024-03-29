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
    int i = 0;

    while(TRUE)
    {
        if(PL_CheckBlockAtWorkstation()){
            i++;
        }
        if(PL_CheckBlockAtDump()){
            i++;
        }
        if(PL_CheckBlockAtRegister()){
            i++;
        }
        if(PL_CheckControlKey()){
            i++;
        }
        if(PL_CheckImpulsAtConveyor()){
            i++;
        }
        if(PL_CheckPistonAtBack()){
            i++;
        }
        if(PL_CheckPistonAtRegister()){
            i++;
        }
    }
}
