#include "global.h"


uint8_t CAN_toSend_Array[2] = {0, 0};
uint16_t CAN_toSend = 0;


bool isEntleerPos(){ return (CAN_received & FS_ENTLEER_ENTLEERPOS_SENSOR)   ? true : false;  }
bool isBOEndePos(){ return (CAN_received & FS_ENTLEER_BO_ENDE_SENSOR)       ? true : false;  }

bool isAnTurmUnten(){ return (CAN_received & FS_ENTLEER_ANTURM_UNTEN)       ? true : false;  }
bool isAnTurmOben(){ return (CAN_received & FS_ENTLEER_ANTURM_OBEN)         ? true : false;  }
bool isAnTurmBelegt(){ return (CAN_received & FS_ENTLEER_ANTURM_SENSOR)     ? true : false;  }
bool isGreifarmOben(){ return (CAN_received & FS_ENTLEER_GREIFRARM_OBEN)    ? true : false;  }
bool isGreifarmUnten(){ return (CAN_received & FS_ENTLEER_GREIFRARM_UNTEN)  ? true : false;  }
bool isAbTurmUnten(){ return (CAN_received & FS_ENTLEER_ABTURM_UNTEN)       ? true : false;  }
bool isAbTurmOben(){ return (CAN_received & FS_ENTLEER_ABTURM_OBEN)         ? true : false;  }
bool isAbTurmBelegt(){ return (CAN_received & FS_ENTLEER_ABTURM_SENSOR)     ? true : false;  }

bool AnTurmErlaubnisDown = true;
bool GreifarmAktiv = false;

bool FS_ENTLEER_BAND_OBEN_ANNAHME = false;
bool FS_ENTLEER_BAND_OBEN_ABGABE = false;


void FS_ENTLEER_Tick() {
    FS_ENTLEER_GREIFARM_TICK = false;

    CAN_received = Data;

    FS_ENTLEER_CAN_received = false;

    /// Anturm darf runterfahren, GreifarmSCHRITT 1 wird aktiviert
    if(isEntleerPos() && GreifarmSchritt == 0) {

        FS_ENTLEER_BAND_OBEN_ANNAHME = false;
        CAN_toSend &= ~FS_ENTLEER_BAND_OBEN;
        CAN_toSend &= ~FS_ENTLEER_ANTURM_BAND;
        AnTurmErlaubnisDown = true;
        GreifarmAktiv = true;
        GreifarmSchritt = 1;
   }


   if(GreifarmSchritt != 0){
        switch(GreifarmSchritt){
            ///Greifarm wird runtergefahren
            case 1: CAN_toSend |= FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                    if(isGreifarmUnten()){
                        CAN_toSend &= ~FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                        GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_TICK = true;
                    }   break;
            ///Greifarm ist unten, greift zu und faehrt sofort hoch
            case 2: CAN_toSend |= FS_ENTLEER_GREIFARM_ZU;
                    CAN_toSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
                    if(isGreifarmOben()){
                        CAN_toSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
                        GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_TICK = true;
                    }   break;
            ///Greifarm ist oben und faehrt sofort runter
            case 3: CAN_toSend |= FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                    if(isGreifarmUnten()){
                        CAN_toSend &= ~FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                        GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_TICK = true;
                    }   break;
            ///Greifarm ist unten, laesst los und faehrt hoch
             case 4: CAN_toSend &= ~FS_ENTLEER_GREIFARM_ZU;
                     CAN_toSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
                     if(isGreifarmOben()){
                        CAN_toSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
                        GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_TICK = true;
                     }   break;
            ///Band faehrt den Klotz bis zum naechsten Sensor
            case 5: CAN_toSend |= FS_ENTLEER_BAND_OBEN;
                    if(isBOEndePos()){
                        CAN_toSend &= ~FS_ENTLEER_BAND_OBEN;
                        GreifarmSchritt = 0;
                        GreifarmAktiv = false;
                        FS_ENTLEER_GREIFARM_TICK = true;
                     }   break;
            default: break;

        }
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
        if(!isEntleerPos() && !isBOEndePos() && GreifarmSchritt == 0){
            CAN_toSend |= FS_ENTLEER_ANTURM_BAND;
            FS_ENTLEER_BAND_OBEN_ANNAHME = true;

        }
    }

    ///Wenn Abturm leer ist, dann Abturm hochfahren
    if(!isAbTurmOben() && !isAbTurmBelegt()){
        CAN_toSend |= FS_ENTLEER_ABTURM_HOCHFAHREN;
    } else {
        CAN_toSend &= ~FS_ENTLEER_ABTURM_HOCHFAHREN;
    }

    ///Wenn BOEnde belegt ist, AbTurm oben ist, beide Baender aktivieren
    if(isBOEndePos() && isAbTurmOben() && !isAbTurmBelegt()){
        CAN_toSend |= FS_ENTLEER_ABTURM_BAND;
        FS_ENTLEER_BAND_OBEN_ABGABE = true;
    }

    ///Wenn Abturm belegt ist, Baender ausschalten und runterfahren
    if(isAbTurmBelegt() && !isAbTurmUnten()){
        CAN_toSend &= ~FS_ENTLEER_ABTURM_BAND;
        FS_ENTLEER_BAND_OBEN_ABGABE = false;
        CAN_toSend |= FS_ENTLEER_ABTURM_RUNTERFAHREN;
    } else {
        CAN_toSend &= ~FS_ENTLEER_ABTURM_RUNTERFAHREN;
    }

    if(!GreifarmAktiv){
        if(!isGreifarmOben()){
            CAN_toSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
        }else{
            CAN_toSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
        }

        if(FS_ENTLEER_BAND_OBEN_ABGABE || FS_ENTLEER_BAND_OBEN_ANNAHME){
            CAN_toSend |= FS_ENTLEER_BAND_OBEN;
        } else {
            CAN_toSend &= ~FS_ENTLEER_BAND_OBEN;
        }
    }



//    if(isBOEndePos()){
//        CAN_toSend &= ~FS_ENTLEER_BAND_OBEN;
//    }


    CAN_toSend_Array[1] = CAN_toSend;
    CAN_toSend_Array[0] = CAN_toSend >> 8;
    CAN_TransmitMsg(FS_ENTLEER_CAN_SEND_ID, CAN_toSend_Array, CAN_DLC_2);
}
