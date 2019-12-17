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
    FS_Servo_init();
    FS_ProjectInit();


    FS_KOM_Init();

    while(TRUE)
    {
        //Kommissionierungsroutine
        if(FS_KOM_CHECK_RoutineCondition())
            FS_KOM_Routine();

        //Entleerungsroutine
        if(FS_ENTLEER_CAN_NewTelegramReceived || FS_ENTLEER_ExtraTickRequest)
            FS_ENTLEER_Tick();
        //Servo Routine
        FS_SERVO_Routine();     //--> Move counting to ISR

    }
}
