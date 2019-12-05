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

    SMOT_Init();


    //SMOT_Goto(SMOT_TOWER_POS, SMOT_SPEED_DEFAULT, X_TO_TOWER, true);
    while(TRUE)
    {
        SMOT_GoUntilRefButton(SMOT_MAXPOS, SMOT_SPEED_DEFAULT, X_TO_BELT, false);
        while(SMOT_Running()){

        }
        SMOT_Goto(SMOT_REF_FROM_TOWER_TO_BELT_LEN, SMOT_SPEED_DEFAULT, X_TO_BELT, false);
         while(SMOT_Running()){

        }
        SMOT_GoUntilRefButton(SMOT_MAXPOS, SMOT_SPEED_DEFAULT, X_TO_TOWER, false);
        while(SMOT_Running()){

        }
        SMOT_Goto(SMOT_TOWER_POS, SMOT_SPEED_DEFAULT, X_TO_TOWER, false);
        //SMOT_Tick();
        while(SMOT_Running()){

        }
    }
}
