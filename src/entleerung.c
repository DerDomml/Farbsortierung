#include "global.h"

/**
 *  Funktionen zum Abfragen der Sensoren
**/

bool FS_ENTLEER_IsEntleerPos(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ENTLEERPOS_SENSOR)   ? true : false;  }
bool FS_ENTLEER_IsBOEndePos(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_BO_ENDE_SENSOR)       ? true : false;  }
bool FS_ENTLEER_IsAnTurmUnten(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ANTURM_UNTEN)       ? true : false;  }
bool FS_ENTLEER_IsAnTurmOben(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ANTURM_OBEN)         ? true : false;  }
bool FS_ENTLEER_IsAnTurmBelegt(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ANTURM_SENSOR)     ? true : false;  }
bool FS_ENTLEER_IsGreifarmOben(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_GREIFRARM_OBEN)    ? true : false;  }
bool FS_ENTLEER_IsGreifarmUnten(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_GREIFRARM_UNTEN)  ? true : false;  }
bool FS_ENTLEER_IsAbTurmUnten(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ABTURM_UNTEN)       ? true : false;  }
bool FS_ENTLEER_IsAbTurmOben(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ABTURM_OBEN)         ? true : false;  }
bool FS_ENTLEER_IsAbTurmBelegt(){ return (FS_ENTLEER_CAN_Received & FS_ENTLEER_ABTURM_SENSOR)     ? true : false;  }

/**
 *  Hauptfunktion; Abfragen aller Sensoren und Ansteuern der Aktoren - "Tick"
**/

void FS_ENTLEER_Tick() {
    /// Zu sendende CAN-Daten als Byte-Array
    static uint8_t FS_ENTLEER_CAN_ToSend_Array[2] = {0, 0};

    /// Zu sendende CAN-Daten als uint 16 fuer leichteren Zugriff
    static uint16_t FS_ENTLEER_CAN_ToSend = 0;

    /// Aktueller Schritt der Greifarmschrittkette
    static uint8_t FS_ENTLEER_GreifarmSchritt = 0;

    /// Freigabe fuer den Annahmeturm
    //static bool FS_ENTLEER_ANTURM_Freigabe = true;

    /// Greifarmschrittkette aktiv?
    static bool FS_ENTLEER_GreifarmAktiv = false;


    /// Ansteuerung des oberen Bandes fuer Zu- oder Abtransport (AnTurm zu Hauptband oder Hauptband zu Abturm)
    static bool FS_ENTLEER_BAND_OBEN_ANNAHME = false;
    static bool FS_ENTLEER_BAND_OBEN_ABGABE = false;

    static uint8_t AnturmSchritt = 0;
    static uint8_t BandObenSchritt = 0;
    static uint8_t AbturmSchritt = 0;

    static bool AnturmAbgabeBereit = false;
    static bool BandObenFreigabeAnturmAnnahmeBereit = false;
    static bool BandObenFreigabeAnturmLeer = false;
    static bool BandObenFreigabeAbturmAbgabeBereit = false;
    static bool AbturmAnnahmeBereit = false;
    static bool AbturmFreigabeBandObenLeer = false;

    /// Anforderung eines sofortigen weiteren Ticks an die main-Funktion (wird bei aktiver Greifarmschrittkette benoetigt)
    FS_ENTLEER_GREIFARM_Tick = false;
    FS_ENTLEER_ANTURM_Tick = false;
    FS_ENTLEER_ABTURM_Tick = false;
    FS_ENTLEER_BAND_OBEN_Tick = false;

    /// Empfangene Daten aus CANBUS
    FS_ENTLEER_CAN_Received = Data;

    /// Neues CAN Telegramm empfangen?
    FS_ENTLEER_CAN_NewTelegramReceived = false;


    switch(AnturmSchritt){
        case 0:
            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_RUNTERFAHREN;
            if(FS_ENTLEER_IsAnTurmUnten()){
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_RUNTERFAHREN;
                AnturmSchritt++;

                ///TODO: Sende an SuKr AnTurm annahmebereit
                FS_ENTLEER_SIMULATION_ANTURM_ANNAHMEBEREIT = true;
            } break;
        case 1:
            if(FS_ENTLEER_SIMULATION_SUKR_BLOCK_BEREIT){ /// Empfange von SukR: Block bereit
                FS_ENTLEER_SIMULATION_SUKR_BLOCK_BEREIT = false;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_BAND;
                AnturmSchritt++;
            } break;
        case 2:
            if(FS_ENTLEER_IsAnTurmBelegt()){
                FS_ENTLEER_SIMULATION_ANTURM_ANNAHMEBEREIT = false;
                AnturmSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_BAND;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_HOCHFAHREN;

                FS_ENTLEER_SIMULATION_SUKR_BLOCK_ERHALTEN = true;
            } break;
        case 3:
            if(FS_ENTLEER_IsAnTurmOben()){

                FS_ENTLEER_SIMULATION_SUKR_BLOCK_ERHALTEN = false;

                AnturmSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_HOCHFAHREN;
                AnturmAbgabeBereit = true;
                ///TODO BandOben Tick ausloesen
            } break;
        case 4:
            if(BandObenFreigabeAnturmAnnahmeBereit){
                AnturmSchritt++;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_BAND;
            } break;
        case 5:
            if(BandObenFreigabeAnturmLeer){
                AnturmSchritt = 0;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_BAND;
                FS_ENTLEER_ANTURM_Tick = true;
                AnturmAbgabeBereit = false;
            } break;

    }

    switch(AbturmSchritt){
        case 0:
            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_HOCHFAHREN;
            if(FS_ENTLEER_IsAbTurmOben()){
                AbturmSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_HOCHFAHREN;
                AbturmAnnahmeBereit = true;
                ///TODO BandOben Tick aktivieren
            } break;
        case 1:
            if(BandObenFreigabeAbturmAbgabeBereit){
                AbturmSchritt++;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_BAND;
            } break;
        case 2:
            if(FS_ENTLEER_IsAbTurmBelegt()){
                AbturmSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_BAND;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_RUNTERFAHREN;
                AbturmFreigabeBandObenLeer = true;
                AbturmAnnahmeBereit = false;
            } break;
        case 3:
            if(FS_ENTLEER_IsAbTurmUnten()){
                AbturmSchritt++;
                AbturmFreigabeBandObenLeer = false;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_RUNTERFAHREN;
                ///TODO Sende Abgabebereitschaft an Wakko
                FS_ENTLEER_SIMULATION_ABTURM_ABGABEBEREIT = true;
            } break;
        case 4:
            if(FS_ENTLEER_SIMULATION_WAKO_BEREIT_FUER_BLOCK){///TODO Empfange Annahmebereitschaft von Wakko
                FS_ENTLEER_SIMULATION_WAKO_BEREIT_FUER_BLOCK = false;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_BAND;
                AbturmSchritt++;
            } break;
        case 5:
            if(!FS_ENTLEER_IsAbTurmBelegt() && FS_ENTLEER_SIMULATION_WAKO_WERKSTUECK_ERHALTEN){///TODO Empfang von Wakko bestaetigt
                FS_ENTLEER_SIMULATION_WAKO_WERKSTUECK_ERHALTEN = false;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_BAND;
                AbturmSchritt = 0;
                FS_ENTLEER_ABTURM_Tick = true;
                FS_ENTLEER_SIMULATION_ABTURM_ABGABEBEREIT = false;
            } break;
    }

    switch(BandObenSchritt){
        case 0:
            if(AnturmAbgabeBereit){
                BandObenSchritt++;
                FS_ENTLEER_BAND_OBEN_Tick = true;
            } break;
        case 1:
            if(!FS_ENTLEER_IsEntleerPos() && FS_ENTLEER_GreifarmSchritt == 0){
                BandObenFreigabeAnturmAnnahmeBereit = true;
                FS_ENTLEER_BAND_OBEN_ANNAHME = true;
                BandObenSchritt++;
                FS_ENTLEER_ANTURM_Tick = true;
            } break;
        case 2:
            if(FS_ENTLEER_IsEntleerPos()){
                BandObenFreigabeAnturmAnnahmeBereit = false;
                FS_ENTLEER_BAND_OBEN_ANNAHME = false;
                BandObenSchritt++;
                FS_ENTLEER_GreifarmAktiv = true;
                BandObenFreigabeAnturmLeer = true;
                FS_ENTLEER_GreifarmSchritt = 1;
                FS_ENTLEER_BAND_OBEN_Tick = true;
            } break;
        case 3:
            if(!FS_ENTLEER_GreifarmAktiv){
                BandObenSchritt++;
                BandObenFreigabeAnturmLeer = false;
                FS_ENTLEER_BAND_OBEN_Tick = true;
            } break;
        case 4:
            if(AbturmAnnahmeBereit){
                BandObenFreigabeAbturmAbgabeBereit = true;
                FS_ENTLEER_BAND_OBEN_ABGABE = true;
                FS_ENTLEER_ABTURM_Tick = true;
                BandObenSchritt++;
            }
        case 5:
            if(AbturmFreigabeBandObenLeer){
                FS_ENTLEER_BAND_OBEN_ABGABE = false;
                BandObenFreigabeAbturmAbgabeBereit = false;
                BandObenSchritt = 0;
            }
            if(AnturmAbgabeBereit && !FS_ENTLEER_IsBOEndePos()){
                BandObenSchritt = 1;
                FS_ENTLEER_BAND_OBEN_ABGABE = false;
                BandObenFreigabeAbturmAbgabeBereit = true;
                FS_ENTLEER_ANTURM_Tick = true;
            }

    }
//    /// Anturm darf runterfahren, GreifarmSCHRITT 1 wird aktiviert
//    if(FS_ENTLEER_IsEntleerPos() && FS_ENTLEER_GreifarmSchritt == 0) {
//
//        FS_ENTLEER_BAND_OBEN_ANNAHME = false;
//        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_BAND_OBEN;
//        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_BAND;
//        FS_ENTLEER_ANTURM_Freigabe = true;
//        FS_ENTLEER_GreifarmAktiv = true;
//        FS_ENTLEER_GreifarmSchritt = 1;
//   }
//
//
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
            case 5: FS_ENTLEER_BAND_OBEN_ANNAHME = true;
                    if(FS_ENTLEER_IsBOEndePos()){
                        FS_ENTLEER_BAND_OBEN_ANNAHME = false;
                        FS_ENTLEER_GreifarmSchritt = 0;
                        FS_ENTLEER_GreifarmAktiv = false;
                        FS_ENTLEER_GREIFARM_Tick = true;
                     }   break;
            default: break;

        }
   }
//
//    /// Anturm faehrt runter
//    if(!FS_ENTLEER_IsAnTurmUnten() && !FS_ENTLEER_IsAnTurmBelegt() && FS_ENTLEER_ANTURM_Freigabe){
//        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_RUNTERFAHREN;
//    } else {
//        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_RUNTERFAHREN;
//        FS_ENTLEER_ANTURM_Freigabe = false;
//    }
//
//    ///Anturm faehrt hoch
//    if(FS_ENTLEER_IsAnTurmUnten() && FS_ENTLEER_IsAnTurmBelegt()){
//        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_HOCHFAHREN;
//    }
//
//    ///Anturm bleibt oben und Baender laufen
//    if(FS_ENTLEER_IsAnTurmOben()){
//
//        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_HOCHFAHREN;
//
//        ///Wenn Anturm oben ist UND das Band leer ist
//        if(!FS_ENTLEER_IsEntleerPos() && !FS_ENTLEER_IsBOEndePos() && FS_ENTLEER_GreifarmSchritt == 0){
//            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_BAND;
//            FS_ENTLEER_BAND_OBEN_ANNAHME = true;
//
//        }
//    }
//
//    ///Wenn Abturm leer ist, dann Abturm hochfahren
//    if(!FS_ENTLEER_IsAbTurmOben() && !FS_ENTLEER_IsAbTurmBelegt()){
//        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_HOCHFAHREN;
//    } else {
//        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_HOCHFAHREN;
//    }
//
//    ///Wenn BOEnde belegt ist, AbTurm oben ist, beide Baender aktivieren
//    if(FS_ENTLEER_IsBOEndePos() && FS_ENTLEER_IsAbTurmOben() && !FS_ENTLEER_IsAbTurmBelegt()){
//        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_BAND;
//        FS_ENTLEER_BAND_OBEN_ABGABE = true;
//    }
//
//    ///Wenn Abturm belegt ist, Baender ausschalten und runterfahren
//    if(FS_ENTLEER_IsAbTurmBelegt() && !FS_ENTLEER_IsAbTurmUnten()){
//        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_BAND;
//        FS_ENTLEER_BAND_OBEN_ABGABE = false;
//        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_RUNTERFAHREN;
//    } else {
//        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_RUNTERFAHREN;
//    }
//
//    if(!FS_ENTLEER_GreifarmAktiv){
//        if(!FS_ENTLEER_IsGreifarmOben()){
//            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
//        }else{
//            FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
//        }

        if(FS_ENTLEER_BAND_OBEN_ABGABE || FS_ENTLEER_BAND_OBEN_ANNAHME){
            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_BAND_OBEN;
        } else {
            FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_BAND_OBEN;
        }




//    if(isBOEndePos()){
//        CAN_toSend &= ~FS_ENTLEER_BAND_OBEN;
//    }


    FS_ENTLEER_CAN_ToSend_Array[1] = FS_ENTLEER_CAN_ToSend;
    FS_ENTLEER_CAN_ToSend_Array[0] = FS_ENTLEER_CAN_ToSend >> 8;
    CAN_TransmitMsg(FS_ENTLEER_CAN_SEND_ID, FS_ENTLEER_CAN_ToSend_Array, CAN_DLC_2);
}
