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

    while(TRUE)
    {
        /**
         * Simulation der Kommunikation mit den anderen Gruppen SuKr & WaKo
        **/
        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)) {
            FS_ENTLEER_SIMULATION_SUKR_BLOCK_BEREIT = true;
            GPIOTick = true;
        }

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)) {
            FS_ENTLEER_SIMULATION_WAKO_BEREIT_FUER_BLOCK = true;
            GPIOTick = true;
        }

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)) {
            FS_ENTLEER_SIMULATION_WAKO_WERKSTUECK_ERHALTEN = true;
            GPIOTick = true;
        }

        if(FS_ENTLEER_CAN_NewTelegramReceived || FS_ENTLEER_GREIFARM_Tick || FS_ENTLEER_ANTURM_Tick || FS_ENTLEER_ABTURM_Tick || FS_ENTLEER_BAND_OBEN_Tick
           || GPIOTick){

            GPIOTick = false;
            FS_ENTLEER_Tick();
        }

        GPIO_WriteBit(GPIOE, GPIO_Pin_9, FS_ENTLEER_SIMULATION_ANTURM_ANNAHMEBEREIT);
        GPIO_WriteBit(GPIOE, GPIO_Pin_10, FS_ENTLEER_SIMULATION_SUKR_BLOCK_ERHALTEN);
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
