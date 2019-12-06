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
    bool button8Prev = false;
    bool button12Prev = false;
    bool button13Prev = false;
    bool button14Prev = false;
    bool button15Prev = false;
    bool GPIOTick = false;

    while(TRUE)
    {
        /**
         * Simulation der Kommunikation mit den anderen Gruppen SuKr & WaKo
        **/
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15) && !button15Prev) {
            FS_SERVO_blockReady = true;
            button15Prev = true;
            GPIOTick = true;
        }
        if(button15Prev && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)){
            button15Prev = false;
        }

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14) && !button14Prev) {
            FS_ENTLEER_SIMULATION_WAKO_BEREIT_FUER_BLOCK = true;
            button14Prev = true;
            GPIOTick = true;
        }
        if(button14Prev && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)){
            button14Prev = false;
        }

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13) && !button13Prev) {
            FS_ENTLEER_SIMULATION_WAKO_WERKSTUECK_ERHALTEN = true;
            button13Prev = true;
            GPIOTick = true;
        }
        if(button13Prev && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)){
            button13Prev = false;
        }

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12) && !button12Prev) {
            FS_SORT_ZYLINDER_start = true;
            button12Prev = true;
            GPIOTick = true;
        }
        if(button12Prev && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_12)){
            button12Prev = false;
        }

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8) && !button8Prev) {
            FS_SORT_ZYLINDER_stop = true;
            button8Prev = true;
            GPIOTick = true;
        }
        if(button8Prev && !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_8)){
            button8Prev = false;
        }


        GPIO_WriteBit(GPIOE, GPIO_Pin_9, FS_ENTLEER_ANTURM_ANNAHMEBEREIT);
        GPIO_WriteBit(GPIOE, GPIO_Pin_10, FS_ENTLEER_ANTURM_BLOCK_ERHALTEN);
        GPIO_WriteBit(GPIOE, GPIO_Pin_11, FS_ENTLEER_SIMULATION_ABTURM_ABGABEBEREIT);

        /**
         * Hauptroutine: Bei entsprechenden Triggern 1 Durchlauf
        **/

        if(FS_ENTLEER_CAN_NewTelegramReceived || FS_ENTLEER_ExtraTickRequest || GPIOTick){

            GPIOTick = false;
            FS_ENTLEER_Tick();
        }

    }
}
