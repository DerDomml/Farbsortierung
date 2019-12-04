#include "global.h"

bool FS_ENTLEER_IsEntleerPos(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ENTLEERPOS_SENSOR)   ? true : false;  }
bool FS_ENTLEER_IsBOEndePos(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_BO_ENDE_SENSOR)       ? true : false;  }

bool FS_ENTLEER_IsAnTurmUnten(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ANTURM_UNTEN)       ? true : false;  }
bool FS_ENTLEER_IsAnTurmOben(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ANTURM_OBEN)         ? true : false;  }
bool FS_ENTLEER_IsAnTurmBelegt(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ANTURM_SENSOR)     ? true : false;  }
bool FS_ENTLEER_IsGreifarmOben(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_GREIFRARM_OBEN)    ? true : false;  }
bool FS_ENTLEER_IsGreifarmUnten(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_GREIFRARM_UNTEN)  ? true : false;  }
bool isAbTurmUnten(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ABTURM_UNTEN)       ? true : false;  }
bool isAbTurmOben(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ABTURM_OBEN)         ? true : false;  }
bool isAbTurmBelegt(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ABTURM_SENSOR)     ? true : false;  }

bool AnTurmErlaubnisDown = true;
bool GreifarmAktiv = false;

bool FS_ENTLEER_BAND_OBEN_ANNAHME = false;
bool FS_ENTLEER_BAND_OBEN_ABGABE = false;


void FS_ENTLEER_Tick() {
    static uint8_t FS_ENTLEER_CAN_ToSend_Array[2] = {0, 0};
    static uint16_t FS_ENTLEER_CAN_ToSend = 0;
    static uint8_t FS_ENTLEER_GreifarmSchritt = 0;

    FS_ENTLEER_GREIFARM_Tick = false;

    FS_ENTLEER_CAN_Received = Data;

    FS_ENTLEER_CAN_received = false;

    /// Anturm darf runterfahren, GreifarmSCHRITT 1 wird aktiviert
    if(FS_ENTLEER_IsEntleerPos() && FS_ENTLEER_GreifarmSchritt == 0) {

        FS_ENTLEER_BAND_OBEN_ANNAHME = false;
        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_BAND_OBEN;
        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_BAND;
        AnTurmErlaubnisDown = true;
        GreifarmAktiv = true;
        FS_ENTLEER_GreifarmSchritt = 1;
   }


   if(FS_ENTLEER_GreifarmSchritt != 0){
        switch(FS_ENTLEER_GreifarmSchritt){
            ///Greifarm wird runtergefahren
            case 1: FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                    if(FS_ENTLEER_IsGreifarmUnten()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                        FS_ENTLEER_GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_Tick = true;
                    }   break;
            ///Greifarm ist unten, greift zu und faehrt sofort hoch
            case 2: FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_ZU;
                    FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
                    if(FS_ENTLEER_IsGreifarmOben()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
                        FS_ENTLEER_GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_Tick = true;
                    }   break;
            ///Greifarm ist oben und faehrt sofort runter
            case 3: FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                    if(FS_ENTLEER_IsGreifarmUnten()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                        FS_ENTLEER_GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_Tick = true;
                    }   break;
            ///Greifarm ist unten, laesst los und faehrt hoch
             case 4: FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_ZU;
                     FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
                     if(FS_ENTLEER_IsGreifarmOben()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
                        FS_ENTLEER_GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_Tick = true;
                     }   break;
            ///Band faehrt den Klotz bis zum naechsten Sensor
            case 5: FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_BAND_OBEN;
                    if(FS_ENTLEER_IsBOEndePos()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_BAND_OBEN;
                        FS_ENTLEER_GreifarmSchritt = 0;
                        GreifarmAktiv = false;
                        FS_ENTLEER_GREIFARM_Tick = true;
                     }   break;
            default: break;

        }
   }

    /// Anturm faehrt runter
    if(!FS_ENTLEER_IsAnTurmUnten() && !FS_ENTLEER_IsAnTurmBelegt() && AnTurmErlaubnisDown){
        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_RUNTERFAHREN;
    } else {
        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_RUNTERFAHREN;
        AnTurmErlaubnisDown = false;
    }

    ///Anturm faehrt hoch
    if(FS_ENTLEER_IsAnTurmUnten() && FS_ENTLEER_IsAnTurmBelegt()){
        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_HOCHFAHREN;
    }

    ///Anturm bleibt oben und Baender laufen
    if(FS_ENTLEER_IsAnTurmOben()){

        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_HOCHFAHREN;

        ///Wenn Anturm oben ist UND das Band leer ist
        if(!FS_ENTLEER_IsEntleerPos() && !FS_ENTLEER_IsBOEndePos() && FS_ENTLEER_GreifarmSchritt == 0){
            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_BAND;
            FS_ENTLEER_BAND_OBEN_ANNAHME = true;

        }
    }

    ///Wenn Abturm leer ist, dann Abturm hochfahren
    if(!isAbTurmOben() && !isAbTurmBelegt()){
        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_HOCHFAHREN;
    } else {
        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_HOCHFAHREN;
    }

    ///Wenn BOEnde belegt ist, AbTurm oben ist, beide Baender aktivieren
    if(FS_ENTLEER_IsBOEndePos() && isAbTurmOben() && !isAbTurmBelegt()){
        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_BAND;
        FS_ENTLEER_BAND_OBEN_ABGABE = true;
    }

    ///Wenn Abturm belegt ist, Baender ausschalten und runterfahren
    if(isAbTurmBelegt() && !isAbTurmUnten()){
        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_BAND;
        FS_ENTLEER_BAND_OBEN_ABGABE = false;
        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_RUNTERFAHREN;
    } else {
        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_RUNTERFAHREN;
    }

    if(!GreifarmAktiv){
        if(!FS_ENTLEER_IsGreifarmOben()){
            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
        }else{
            FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
        }

        if(FS_ENTLEER_BAND_OBEN_ABGABE || FS_ENTLEER_BAND_OBEN_ANNAHME){
            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_BAND_OBEN;
        } else {
            FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_BAND_OBEN;
        }
    }



//    if(isBOEndePos()){
//        CAN_toSend &= ~FS_ENTLEER_BAND_OBEN;
//    }


    FS_ENTLEER_CAN_ToSend_Array[1] = FS_ENTLEER_CAN_ToSend;
    FS_ENTLEER_CAN_ToSend_Array[0] = FS_ENTLEER_CAN_ToSend >> 8;
    CAN_TransmitMsg(FS_ENTLEER_CAN_SEND_ID, FS_ENTLEER_CAN_ToSend_Array, CAN_DLC_2);
}
