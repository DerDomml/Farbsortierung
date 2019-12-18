#include "global.h"

enum AnTurmState {
    ANTURM_AUSGANGSZUSTAND = 0,
    ANTURM_ANNAHMEBEREIT = 1,
    ANTURM_BLOCK_ERHALTEN = 2,
    ANTURM_HOCH = 3,
    ANTURM_ABGABEBEREIT = 4,
    ANTURM_FERTIG = 5
};

enum ABTurmState {
    ABTURM_AUSGANGSZUSTAND = 0,
    ABTURM_ANNAHMEBEREIT = 1,
    ABTURM_RUNTERFAHREN = 2,
    ABTURM_ABGABEBEREIT = 3,
    ABTURM_ABGABE_AN_SCHRITT = 4,
    ABTURM_FERTIG = 5
};

enum BandObenState {
    BANDOBEN_AUSGANGSZUSTAND = 0,
    BANDOBEN_WARTE_AUF_ANTURM = 1,
    BANDOBEN_ANNAHMEBEREIT = 2,
    BANDOBEN_BLOCK_ERHALTEN = 3,
    BANDOBEN_WARTE_AUF_GREIFARM = 4,
    BANDOBEN_ABGABEBEREIT = 5,
    BANDOBEN_FERTIG = 6
};

enum GreifarmState {
    GREIFARM_AUSGANGSZUSTAND = 0,
    GREIFARM_RUNTERFAHREN_1 = 1,
    GREIFARM_ZUGRIFF = 2,
    GREIFARM_RUNTERFAHREN_2 = 3,
    GREIFARM_LOSLASSEN = 4,
    GREIFARM_FERTIG = 5
};

bool FS_ENTLEER_GREIFARM_Tick;
bool FS_ENTLEER_ANTURM_Tick;
bool FS_ENTLEER_ABTURM_Tick;
bool FS_ENTLEER_BAND_OBEN_Tick;

static uint16_t FS_ENTLEER_DI_INData;
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

void FS_ENTLEER_NewData(CanRxMsg RxMessage)
{
    FS_ENTLEER_DI_INData = RxMessage.Data[1];
    FS_ENTLEER_DI_INData |= (RxMessage.Data[0] << 8);
    FS_ENTLEER_CAN_NewTelegramReceived = true;
}

/**
 *  Hauptfunktion; Abfragen aller Sensoren und Ansteuern der Aktoren - "Tick"
**/

void FS_ENTLEER_Tick() {
    /// Zu sendende CAN-Daten als Byte-Array
    static uint8_t FS_ENTLEER_CAN_ToSend_Array[2] = {0, 0};

    /// Zu sendende CAN-Daten als uint 16 fuer leichteren Zugriff
    static uint16_t FS_ENTLEER_CAN_ToSend = 0x0000;
    static uint16_t FS_ENTLEER_CAN_ToSend_prev = 0x0001;
    /// Greifarmschrittkette aktiv?
    static bool FS_ENTLEER_GreifarmAktiv = false;

    /// Ansteuerung des oberen Bandes fuer Zu- oder Abtransport (AnTurm zu Hauptband oder Hauptband zu Abturm)
    static bool FS_ENTLEER_BAND_OBEN_ANNAHME = false;
    static bool FS_ENTLEER_BAND_OBEN_ABGABE = false;

    ///Aktueller Schritt in den Schrittketten
    static uint8_t FS_ENTLEER_AnturmSchritt = ANTURM_AUSGANGSZUSTAND;
    static uint8_t FS_ENTLEER_BandObenSchritt = ABTURM_AUSGANGSZUSTAND;
    static uint8_t FS_ENTLEER_AbturmSchritt = BANDOBEN_AUSGANGSZUSTAND;
    static uint8_t FS_ENTLEER_GreifarmSchritt = GREIFARM_AUSGANGSZUSTAND;

    /// Informationsaustausch zwischen den Schrittketten
    static bool FS_ENTLEER_ANTURM_AbgabeBereit = false;
    static bool FS_ENTLEER_BAND_OBEN_AnnahmeBereit = false;
    static bool FS_ENTLEER_BAND_OBEN_BlockErhalten = false;
    static bool FS_ENTLEER_BAND_OBEN_AbgabeBereit = false;
    static bool FS_ENTLEER_ABTURM_AnnahmeBereit = false;
    static bool FS_ENTLEER_ABTURM_BlockErhalten = false;

    /// Anforderung eines sofortigen weiteren Ticks an die main-Funktion
    FS_ENTLEER_GREIFARM_Tick = false;
    FS_ENTLEER_ANTURM_Tick = false;
    FS_ENTLEER_ABTURM_Tick = false;
    FS_ENTLEER_BAND_OBEN_Tick = false;

    /// Empfangene Daten aus CANBUS
    FS_ENTLEER_CAN_Received = FS_ENTLEER_DI_INData;

    /// Neues CAN Telegramm empfangen?
    FS_ENTLEER_CAN_NewTelegramReceived = false;

    ///Reaktionen auf Befehle der FS_SORT-Gruppe (Zylinder rotieren/Zylinder stoppen)
    if(FS_SORT_ZYLINDER_start) {
        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ZYLINDER_ROTIEREN;
    }

    else{
        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ZYLINDER_ROTIEREN;
    }

    ///AnTurm Schrittkette
    switch(FS_ENTLEER_AnturmSchritt){
        /// Default Schritt: Fahre in Startposition, wenn noch nicht unten
        case ANTURM_AUSGANGSZUSTAND:
            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_RUNTERFAHREN;
            /// Runterfahrt stoppen, wenn unten angekommen - Sende Bereitschaft an FS_SERVO
            if(FS_ENTLEER_IsAnTurmUnten()){
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_RUNTERFAHREN;
                FS_ENTLEER_AnturmSchritt++;
                FS_ENTLEER_ANTURM_ANNAHMEBEREIT = true;
            } break;
        /// Warten auf Bereitschaft von FS_SERVO
        case ANTURM_ANNAHMEBEREIT:
            /// Empfange von SukR: Block bereit - Bandlauf starten
            if(FS_SERVO_blockReady){
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_BAND;
                FS_ENTLEER_AnturmSchritt++;
            } break;
        ///Warten auf Empfang des Blocks
        case ANTURM_BLOCK_ERHALTEN:
            ///Bei Empfang des Blocks: Empfang bestaetigen an FS_SERVO; Turm hochfahren, Bandlauf stoppen
//            TIM_Cmd(TIM6, ENABLE);
//            if(FS_ENTLEER_WERKSTUECKVERLUST) {
//                FS_ENTLEER_AnturmSchritt = 0;
//                FS_ENTLEER_WERKSTUECKVERLUST = false;
//                FS_ENTLEER_ANTURM_Tick = true;
//                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_BAND;
//            }
            if(FS_ENTLEER_IsAnTurmBelegt()){
//                TIM_Cmd(TIM6, DISABLE);
//                TIM_SetCounter(TIM6, 0);
                FS_ENTLEER_ANTURM_ANNAHMEBEREIT = false;
                FS_ENTLEER_AnturmSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_BAND;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_HOCHFAHREN;
                FS_ENTLEER_ANTURM_BLOCK_ERHALTEN = true;
            } break;
        ///Warten bis Turm oben ist
        case ANTURM_HOCH:
            /// Wenn Turm oben ist, nicht mehr hochfahren, Abgabebereitschaft signalisieren an BAND_OBEN
            FS_ENTLEER_ANTURM_BLOCK_ERHALTEN = false;
            if(FS_ENTLEER_IsAnTurmOben()){
                FS_ENTLEER_AnturmSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_HOCHFAHREN;
                FS_ENTLEER_ANTURM_AbgabeBereit = true;
            }  else if(!FS_ENTLEER_IsAnTurmBelegt()){
                FS_ENTLEER_AnturmSchritt = ANTURM_AUSGANGSZUSTAND;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_HOCHFAHREN;
                FS_ENTLEER_ANTURM_Tick = true;
            }   break;
        ///Warten auf Annahmebereitschaft von BAND_OBEN
        case ANTURM_ABGABEBEREIT:
            ///Bandlauf Starten
            if(FS_ENTLEER_BAND_OBEN_AnnahmeBereit){
                FS_ENTLEER_AnturmSchritt++;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ANTURM_BAND;
            } break;
        ///Warten auf Empfangsbestaetigung von BAND_OBEN
        case ANTURM_FERTIG:
            ///Bandlauf stoppen, zurueck auf Ausgangszustand
//            TIM_Cmd(TIM6, ENABLE);
//            if(FS_ENTLEER_WERKSTUECKVERLUST) {
//                FS_ENTLEER_AnturmSchritt = 0;
//                FS_ENTLEER_ANTURM_Tick = true;
//                FS_ENTLEER_ANTURM_AbgabeBereit = false;
//                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_BAND;
//            }
            if(FS_ENTLEER_BAND_OBEN_BlockErhalten){
//                TIM_Cmd(TIM6, DISABLE);
//                TIM_SetCounter(TIM6, 0);
                FS_ENTLEER_AnturmSchritt = 0;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ANTURM_BAND;
                FS_ENTLEER_ANTURM_Tick = true;
                FS_ENTLEER_ANTURM_AbgabeBereit = false;
            } break;

    }

    ///AbTurm Schrittkette
    switch(FS_ENTLEER_AbturmSchritt){
        ///Default Schritt: Fahre in Startposition, wenn noch nicht oben
        case ABTURM_AUSGANGSZUSTAND:
            FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_HOCHFAHREN;
            ///Wenn Oben, nicht mehr hochfahren, Annahmebereitschaft an BAND_OBEN signalisieren
            if(FS_ENTLEER_IsAbTurmOben()){
                FS_ENTLEER_AbturmSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_HOCHFAHREN;
                FS_ENTLEER_ABTURM_AnnahmeBereit = true;
            } break;
        ///Warten auf Abgabebereitschaft von BAND_OBEN
        case ABTURM_ANNAHMEBEREIT:
            ///Bandlauf aktivieren
            if(FS_ENTLEER_BAND_OBEN_AbgabeBereit){
                FS_ENTLEER_AbturmSchritt++;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_BAND;
            } break;
        ///Warten auf Empfang des Blocks
        case ABTURM_RUNTERFAHREN:
            ///Empfang an BAND_OBEN bestaetigen, Bandlauf stoppen und Runterfahren
//            TIM_Cmd(TIM6, ENABLE);
//            if(FS_ENTLEER_WERKSTUECKVERLUST) {
//                FS_ENTLEER_AbturmSchritt = 0;
//                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_BAND;
//                FS_ENTLEER_ABTURM_Tick = true;
//            }
            if(FS_ENTLEER_IsAbTurmBelegt()){
//                TIM_Cmd(TIM6, DISABLE);
//                TIM_SetCounter(TIM6, 0);
                FS_ENTLEER_AbturmSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_BAND;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_RUNTERFAHREN;
                FS_ENTLEER_ABTURM_BlockErhalten = true;
                FS_ENTLEER_ABTURM_AnnahmeBereit = false;
            } break;
        ///Warten bis Aufzug unten ist
        case ABTURM_ABGABEBEREIT:
            ///Fahrt stoppen, Abgabebereitschaft an FS_SCHRITT signalisieren
            if(FS_ENTLEER_IsAbTurmUnten()){
                FS_ENTLEER_AbturmSchritt++;
                FS_ENTLEER_ABTURM_BlockErhalten = false;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_RUNTERFAHREN;
                FS_ENTLEER_ABTURM_ABGABEBEREIT = true;
                FS_ENTLEER_ABTURM_Tick = true;
            }  else if(!FS_ENTLEER_IsAbTurmBelegt()){
                FS_ENTLEER_AbturmSchritt = ABTURM_AUSGANGSZUSTAND;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_RUNTERFAHREN;
                FS_ENTLEER_ABTURM_BlockErhalten = false;
                FS_ENTLEER_ABTURM_Tick = true;
            } break;
        ///Warten auf Annahmebereitschaft von FS_SCHRITT
        case ABTURM_ABGABE_AN_SCHRITT:
            ///Bandlauf starten
            if(FS_SCHRITT_ANNAHMEBEREIT){
                FS_SCHRITT_ANNAHMEBEREIT = false;
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_ABTURM_BAND;
                FS_ENTLEER_AbturmSchritt++;
            }  else if(!FS_ENTLEER_IsAbTurmBelegt()){
                FS_ENTLEER_AbturmSchritt = ABTURM_AUSGANGSZUSTAND;
                FS_ENTLEER_ABTURM_Tick = true;
            } break;
        ///Warten auf Empfangsbestaetigung von FS_SCHRITT
        case ABTURM_FERTIG:
            ///Bandlauf stoppen, zurueck auf Schritt 0
//            TIM_Cmd(TIM6, ENABLE);
//            if(FS_ENTLEER_WERKSTUECKVERLUST) {
//                FS_ENTLEER_AbturmSchritt = 0;
//                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_BAND;
//                FS_ENTLEER_ABTURM_Tick = true;
//            }
            if(!FS_ENTLEER_IsAbTurmBelegt() && FS_SCHRITT_BLOCK_ERHALTEN){
//                TIM_Cmd(TIM6, DISABLE);
//                TIM_SetCounter(TIM6, 0);
                FS_SCHRITT_BLOCK_ERHALTEN = false;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_ABTURM_BAND;
                FS_ENTLEER_AbturmSchritt = ABTURM_AUSGANGSZUSTAND;
                FS_ENTLEER_ABTURM_Tick = true;
                FS_ENTLEER_ABTURM_ABGABEBEREIT = false;
            } break;
    }

    ///Band Oben Schrittkette
    switch(FS_ENTLEER_BandObenSchritt){
        case BANDOBEN_AUSGANGSZUSTAND:
            if(FS_ENTLEER_IsGreifarmOben()){
                FS_ENTLEER_BandObenSchritt++;
                FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
            } else {
                FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
            }
        ///Warten auf Abgabebereitschaft von Anturm
        case BANDOBEN_WARTE_AUF_ANTURM:
            if(FS_ENTLEER_ANTURM_AbgabeBereit){
                FS_ENTLEER_BandObenSchritt++;
                FS_ENTLEER_BAND_OBEN_Tick = true;
            } break;
        ///Wenn BandOben leer ist, signalisiere Annahmebereitschaft an Anturm, Bandlauf Starten
        case BANDOBEN_ANNAHMEBEREIT:
            if(!FS_ENTLEER_IsEntleerPos() && FS_ENTLEER_GreifarmSchritt == GREIFARM_AUSGANGSZUSTAND){
                FS_ENTLEER_BAND_OBEN_AnnahmeBereit = true;
                FS_ENTLEER_BAND_OBEN_ANNAHME = true;
                FS_ENTLEER_BandObenSchritt++;
                FS_ENTLEER_ANTURM_Tick = true;
            } break;
        ///Warten auf Empfang des Blocks
        case BANDOBEN_BLOCK_ERHALTEN:
            ///Bandlauf stoppen, Greifarmschrittkette aktivieren, Empfang des Blocks an Anturm bestaetigen
//            if(FS_ENTLEER_WERKSTUECKVERLUST) {
//                FS_ENTLEER_BandObenSchritt = 0;
//                FS_ENTLEER_BAND_OBEN_ANNAHME = false;
//                FS_ENTLEER_BAND_OBEN_Tick = true;
//            }
            if(FS_ENTLEER_IsEntleerPos()){
                FS_ENTLEER_BAND_OBEN_AnnahmeBereit = false;
                FS_ENTLEER_BAND_OBEN_ANNAHME = false;
                FS_ENTLEER_BandObenSchritt++;
                FS_ENTLEER_GreifarmAktiv = true;
                FS_ENTLEER_BAND_OBEN_BlockErhalten = true;
                FS_ENTLEER_GreifarmSchritt = GREIFARM_RUNTERFAHREN_1;
                FS_ENTLEER_BAND_OBEN_Tick = true;
            } break;
        ///Warten auf Durchlauf der Greifarmschrittkette (Block sollte auf BOEnde Position sein)
        case BANDOBEN_WARTE_AUF_GREIFARM:
            if(!FS_ENTLEER_GreifarmAktiv){
                FS_ENTLEER_BAND_OBEN_BlockErhalten = false;
                FS_ENTLEER_BandObenSchritt++;
                FS_ENTLEER_BAND_OBEN_Tick = true;
            } break;
        ///Warten auf Annahmebereitschaft von Abturm
        case BANDOBEN_ABGABEBEREIT:
            ///Abgabebereitschaft an Abturm signalisieren, Bandlauf starten
            if(FS_ENTLEER_ABTURM_AnnahmeBereit){
                FS_ENTLEER_BAND_OBEN_AbgabeBereit = true;
                FS_ENTLEER_BAND_OBEN_ABGABE = true;
                FS_ENTLEER_ABTURM_Tick = true;
                FS_ENTLEER_BandObenSchritt++;
            }
        ///Warten auf Empfangsbestaetigung von Abturm oder Abgabebereitschaft von Anturm
        case BANDOBEN_FERTIG:
//            if(FS_ENTLEER_WERKSTUECKVERLUST) {
//                FS_ENTLEER_WERKSTUECKVERLUST = false;
//                FS_ENTLEER_BandObenSchritt = 0;
//                FS_ENTLEER_BAND_OBEN_ABGABE = false;
//                FS_ENTLEER_BAND_OBEN_Tick = true;
//            }
            ///Bandlauf stoppen, zurueck auf Schritt 0
            if(FS_ENTLEER_ABTURM_BlockErhalten){
                FS_ENTLEER_BAND_OBEN_ABGABE = false;
                FS_ENTLEER_BAND_OBEN_AbgabeBereit = false;
                FS_ENTLEER_BandObenSchritt = BANDOBEN_AUSGANGSZUSTAND;
            }
            /// Bandlauf stoppen und auf Schritt 1
            if(FS_ENTLEER_ANTURM_AbgabeBereit && !FS_ENTLEER_IsBOEndePos()){
                FS_ENTLEER_BandObenSchritt = BANDOBEN_WARTE_AUF_ANTURM;
                FS_ENTLEER_BAND_OBEN_ABGABE = false;
                FS_ENTLEER_BAND_OBEN_AbgabeBereit = false;
                FS_ENTLEER_ANTURM_Tick = true;
            }

    }

    ///Greifarmschrittkette
    if(FS_ENTLEER_GreifarmSchritt != GREIFARM_AUSGANGSZUSTAND){
        switch(FS_ENTLEER_GreifarmSchritt){
            ///Greifarm wird runtergefahren
            case GREIFARM_RUNTERFAHREN_1: FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                    if(FS_ENTLEER_IsGreifarmUnten()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                        FS_ENTLEER_GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_Tick = true;
                    }   break;
            ///Greifarm ist unten, greift zu und faehrt sofort hoch
            case GREIFARM_ZUGRIFF: FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_ZU;
                    FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
                    if(FS_ENTLEER_IsGreifarmOben()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
                        FS_ENTLEER_GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_Tick = true;
                    }   break;
            ///Greifarm ist oben und faehrt sofort runter
            case GREIFARM_RUNTERFAHREN_2: FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                    if(FS_ENTLEER_IsGreifarmUnten()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_RUNTERFAHREN;
                        FS_ENTLEER_GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_Tick = true;
                    }   break;
            ///Greifarm ist unten, laesst los und faehrt hoch
             case GREIFARM_LOSLASSEN: FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_ZU;
                     FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_GREIFARM_HOCHFAHREN;
                     if(FS_ENTLEER_IsGreifarmOben()){
                        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_GREIFARM_HOCHFAHREN;
                        FS_ENTLEER_GreifarmSchritt++;
                        FS_ENTLEER_GREIFARM_Tick = true;
                     }   break;
            ///Band faehrt den Klotz bis zum naechsten Sensor
            case GREIFARM_FERTIG: FS_ENTLEER_BAND_OBEN_ANNAHME = true;
//                TIM_Cmd(TIM6, ENABLE);
//                if(FS_ENTLEER_WERKSTUECKVERLUST) {
//                    FS_ENTLEER_BandObenSchritt = 0;
//                    FS_ENTLEER_GreifarmSchritt = 0;
//                    FS_ENTLEER_GreifarmAktiv = false;
//                    FS_ENTLEER_BAND_OBEN_ANNAHME = false;
//                    FS_ENTLEER_GREIFARM_Tick = true;
//                }
                if(FS_ENTLEER_IsBOEndePos()){
//                    TIM_Cmd(TIM6, DISABLE);
//                    TIM_SetCounter(TIM6, 0);
                    FS_ENTLEER_BAND_OBEN_ANNAHME = false;
                    FS_ENTLEER_GreifarmSchritt = GREIFARM_AUSGANGSZUSTAND;
                    FS_ENTLEER_GreifarmAktiv = false;
                    FS_ENTLEER_GREIFARM_Tick = true;
                 }   break;
                default: break;
        }
    }
//    FS_ENTLEER_WERKSTUECKVERLUST = false;

    ///Hilfsvariablen fuer BAND_OBEN Bandlauf
    if(FS_ENTLEER_BAND_OBEN_ABGABE || FS_ENTLEER_BAND_OBEN_ANNAHME){
        FS_ENTLEER_CAN_ToSend |= FS_ENTLEER_BAND_OBEN;
    } else {
        FS_ENTLEER_CAN_ToSend &= ~FS_ENTLEER_BAND_OBEN;
    }

    ///Hilfsvariable fuer Extra Ticks
    FS_ENTLEER_ExtraTickRequest = FS_ENTLEER_ABTURM_Tick || FS_ENTLEER_ANTURM_Tick || FS_ENTLEER_BAND_OBEN_Tick || FS_ENTLEER_GREIFARM_Tick;

    ///Umwandlung der uint16 zu einem 2-Byte Array und Versand des CAN-Telegramms



    if(FS_ENTLEER_CAN_ToSend != FS_ENTLEER_CAN_ToSend_prev){
        FS_ENTLEER_CAN_ToSend_prev = FS_ENTLEER_CAN_ToSend;
        FS_ENTLEER_CAN_ToSend_Array[1] = FS_ENTLEER_CAN_ToSend;
        FS_ENTLEER_CAN_ToSend_Array[0] = FS_ENTLEER_CAN_ToSend >> 8;
        CAN_TransmitMsg(FS_ENTLEER_DO_11Bit_ID, FS_ENTLEER_CAN_ToSend_Array, CAN_DLC_2);
    }
}
