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
    uint8_t bytes[] = {0x01, 0x2C};

    ProjectInit();

    int testus = Bytes_To_Int(bytes[0], bytes[1]);

    //int i = 0;

    while(TRUE)
    {
        /*if(PL_CheckBlockAtWorkstation()){
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

            }*/
            //GPIO_Write(GPIOD, (uint16_t) Data[0]);

            SMOT_Update();
    }
}
