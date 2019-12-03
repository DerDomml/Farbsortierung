#include "global.h"


uint8_t CAN_toSend_Array[2] = {0, 0};
uint16_t CAN_toSend = 0;


bool isEntleerPos(){ return (CAN_received & FS_ENTLEER_ENTLEERPOS_SENSOR)   ? true : false;  }
bool isBOEndePos(){ return (CAN_received & FS_ENTLEER_BO_ENDE_SENSOR)       ? true : false;  }

bool isAnTurmUnten(){ return (CAN_received & FS_ENTLEER_ANTURM_UNTEN)       ? true : false;  }
bool isAnTurmOben(){ return (CAN_received & FS_ENTLEER_ANTURM_OBEN)         ? true : false;  }
bool isAnTurmBelegt(){ return (CAN_received & FS_ENTLEER_ANTURM_SENSOR)     ? true : false;  }

bool AnTurmErlaubnisDown = true;


void FS_ENTLEER_Tick() {

    CAN_received = Data;

    FS_ENTLEER_CAN_received = false;

    /// Anturm darf runterfahren
    if(isEntleerPos()) {
        CAN_toSend &= ~FS_ENTLEER_BAND_OBEN;
        CAN_toSend &= ~FS_ENTLEER_ANTURM_BAND;
        AnTurmErlaubnisDown = true;
   }

    /// Anturm faehrt runter
    if(!isAnTurmUnten() && !isAnTurmBelegt() && AnTurmErlaubnisDown){
        CAN_toSend |= FS_ENTLEER_ANTURM_RUNTERFAHREN;
    } else {
        CAN_toSend &= ~FS_ENTLEER_ANTURM_RUNTERFAHREN;
        AnTurmErlaubnisDown = false;
    }

    ///Anturm faehrt hoch
    if(isAnTurmUnten() && isAnTurmBelegt()){
        CAN_toSend |= FS_ENTLEER_ANTURM_HOCHFAHREN;
    }

    ///Anturm bleibt oben und Baender laufen
    if(isAnTurmOben()){

        CAN_toSend &= ~FS_ENTLEER_ANTURM_HOCHFAHREN;

        ///Wenn Anturm oben ist UND das Band leer ist
        if(!isEntleerPos() && !isBOEndePos()){
            CAN_toSend |= FS_ENTLEER_ANTURM_BAND;
            CAN_toSend |= FS_ENTLEER_BAND_OBEN;

        }
    }


//
//    if(isBOEndePos()){
//        CAN_toSend &= ~FS_ENTLEER_BAND_OBEN;
//    }


    CAN_toSend_Array[1] = CAN_toSend;
    CAN_toSend_Array[0] = CAN_toSend >> 8;
    CAN_TransmitMsg(FS_ENTLEER_CAN_SEND_ID, CAN_toSend_Array, CAN_DLC_2);
}
