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

    while(TRUE)
    {
        /**
         * Simulation der Kommunikation mit den anderen Gruppen SuKr & WaKo
        *//
        if(GPIO_ReadInputDataif(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_15)) {
            FS_ENTLEER_SIMULATION_SUKR_BLOCK_BEREIT = true;
        } else {
            FS_ENTLEER_SIMULATION_SUKR_BLOCK_BEREIT = false;
        }

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_14)) {
            FS_ENTLEER_SIMULATION_WAKO_BEREIT_FUER_BLOCK = true;
        } else {
            FS_ENTLEER_SIMULATION_WAKO_BEREIT_FUER_BLOCK = false;
        }

        if(GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_13)) {
            FS_ENTLEER_SIMULATION_WAKO_WERKSTUECK_ERHALTEN = true;
        } else {
            FS_ENTLEER_SIMULATION_WAKO_WERKSTUECK_ERHALTEN = false;
        }

        if(FS_ENTLEER_CAN_NewTelegramReceived || FS_ENTLEER_GREIFARM_Tick){
            FS_ENTLEER_Tick();
        }

        GPIO_WriteBit(GPIOE, GPIO_PIN_9, FS_ENTLEER_SIMULATION_ANTURM_ANNAHMEBEREIT);
        GPIO_WriteBit(GPIOE, GPIO_PIN_10, FS_ENTLEER_SIMULATION_WAKO_BLOCK_ERHALTEN);
        GPIO_WriteBit(GPIOE, GPIO_PIN_11, FS_ENTLEER_SIMULATION_ABTURM_ABGABEBEREIT);
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
