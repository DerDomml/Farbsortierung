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

    //int i = 0;

    //SMOT_Goto(SMOT_ENDPOS, SMOT_SPEED_DEFAULT, 1);

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
        if(GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0) && !GPIOD_0_prev)
        {
            ENT_Ready_To_Give = true;
            GPIOD_0_prev = true;
        }
        else if(!GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_0) && GPIOD_0_prev)
        {
            GPIOD_0_prev = false;
        }

        SMOT_Tick();
    }
}
