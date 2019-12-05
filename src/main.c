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
    FS_ENTLEER_GREIFARM_Tick = false;
    bool GPIOTick = false;
    bool button13Prev = false;
    bool button14Prev = false;
    bool button15Prev = false;

    while(TRUE)
    {
        /**
         * Simulation der Kommunikation mit den anderen Gruppen SuKr & WaKo
        **/
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15) && !button15Prev) {
            FS_SERVO_BlockReady = true;
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

        if(FS_ENTLEER_CAN_NewTelegramReceived || FS_ENTLEER_GREIFARM_Tick || FS_ENTLEER_ANTURM_Tick || FS_ENTLEER_ABTURM_Tick || FS_ENTLEER_BAND_OBEN_Tick
           || GPIOTick){

            GPIOTick = false;
            FS_ENTLEER_Tick();
        }

        GPIO_WriteBit(GPIOE, GPIO_Pin_9, FS_ENTLEER_ANTURM_ANNAHMEBEREIT);
        GPIO_WriteBit(GPIOE, GPIO_Pin_10, FS_ENTLEER_ANTURM_BLOCK_ERHALTEN);
        GPIO_WriteBit(GPIOE, GPIO_Pin_11, FS_ENTLEER_SIMULATION_ABTURM_ABGABEBEREIT);
//        uint8_t Greifarmhoch[2] = {0x02, 0x00};
//        uint8_t AnTurmHoch[2] = {0x80, 0x00};
//        uint8_t notaus[2] = {0x00, 0x00};
//
//        CAN_TransmitMsg(0x213, Greifarmhoch, 2);
//        CAN_TransmitMsg(0x213, notaus, 2);
//        CAN_TransmitMsg(0x213, AnTurmHoch, 2);
//        CAN_TransmitMsg(0x213, notaus, 2);
    }
}
