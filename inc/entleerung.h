/**
 *  Defines fuer Sensoren
**/

#define FS_ENTLEER_ABTURM_OBEN          0x0001
#define FS_ENTLEER_ABTURM_UNTEN         0x0002
#define FS_ENTLEER_ABTURM_SENSOR        0x0004
#define FS_ENTLEER_ANTURM_OBEN          0x0100
#define FS_ENTLEER_ANTURM_UNTEN         0x0200
#define FS_ENTLEER_ANTURM_SENSOR        0x0400
#define FS_ENTLEER_ENTLEERPOS_SENSOR    0x0800
#define FS_ENTLEER_BO_ENDE_SENSOR       0x1000
#define FS_ENTLEER_GREIFRARM_OBEN            0x2000
#define FS_ENTLEER_GREIFRARM_UNTEN           0x4000

/**
 *  Defines fuer Aktoren
**/

#define FS_ENTLEER_ANTURM_HOCHFAHREN        0x8000
#define FS_ENTLEER_ANTURM_RUNTERFAHREN      0x4000
#define FS_ENTLEER_ANTURM_BAND              0x2000
#define FS_ENTLEER_BAND_OBEN                0x0800
#define FS_ENTLEER_GREIFARM_HOCHFAHREN     0x0200
#define FS_ENTLEER_GREIFARM_RUNTERFAHREN   0x0100
#define FS_ENTLEER_GREIFARM_ZU           	0x0004
#define FS_ENTLEER_ABTURM_HOCHFAHREN        0x0040
#define FS_ENTLEER_ABTURM_RUNTERFAHREN      0x0020
#define FS_ENTLEER_ABTURM_BAND          	0x0010
#define FS_ENTLEER_ZYLINDER_ROTIEREN        0x0080


/**
 *  Globale Variablen
**/

uint16_t FS_ENTLEER_CAN_Received;
bool FS_ENTLEER_GREIFARM_Tick;
bool FS_ENTLEER_ANTURM_Tick;
bool FS_ENTLEER_ABTURM_Tick;
bool FS_ENTLEER_BAND_OBEN_Tick;

/**
 *  Funktionen zum Abfragen der Sensoren
**/

bool FS_ENTLEER_IsEntleerPos();
bool FS_ENTLEER_IsBOEndePos();
bool FS_ENTLEER_IsAnTurmUnten();
bool FS_ENTLEER_IsAnTurmOben();
bool FS_ENTLEER_IsAnTurmBelegt();
bool FS_ENTLEER_IsGreifarmOben();
bool FS_ENTLEER_IsGreifarmUnten();
bool FS_ENTLEER_IsAbTurmUnten();
bool FS_ENTLEER_IsAbTurmOben();
bool FS_ENTLEER_IsAbTurmBelegt();

/**
 *  Hauptfunktion; Abfragen aller Sensoren und Ansteuern der Aktoren - "Tick"
**/

/**
 *  temporaere Variablen zur Simulation der Kommunikation mit anderen Gruppen
**/


bool FS_ENTLEER_SIMULATION_WAKO_BEREIT_FUER_BLOCK;
bool FS_ENTLEER_SIMULATION_WAKO_WERKSTUECK_ERHALTEN;


bool FS_ENTLEER_SIMULATION_ABTURM_ABGABEBEREIT;

/**
 *  Variablen zur Kommunikation mit anderen Gruppen
**/
bool FS_SERVO_BlockReady;

bool FS_ENTLEER_ANTURM_ANNAHMEBEREIT;
bool FS_ENTLEER_ANTURM_BLOCK_ERHALTEN;


void FS_ENTLEER_Tick();
