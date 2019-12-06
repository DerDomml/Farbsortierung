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

        //SMOT_GoUntilRefButton(SMOT_MAXPOS, SMOT_SPEED_DEFAULT, X_TO_BELT, false);


    //SMOT_Goto(SMOT_TOWER_POS, SMOT_SPEED_DEFAULT, X_TO_TOWER, true);
    while(TRUE)
    {

                SMOT_Tick();
    }
}
