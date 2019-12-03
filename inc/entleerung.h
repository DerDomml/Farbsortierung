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
#define FS_ENTLEER_GREIFRARM_HOCHFAHREN     0x0200
#define FS_ENTLEER_GREIFRARM_RUNTERFAHREN   0x0100
#define FS_ENTLEER_GREIFRARM_ZU           	0x0004
#define FS_ENTLEER_ABTURM_HOCHFAHREN        0x0040
#define FS_ENTLEER_ABTURM_RUNTERFAHREN      0x0020
#define FS_ENTLEER_ABTURM_BAND          	0x0010
#define FS_ENTLEER_ZYLINDER_ROTIEREN        0x0080

uint16_t CAN_received;
uint16_t CAN_toSend;

uint8_t GreifarmSchritt;

bool isEntleerPos();
bool isBOEndePos();
void FS_ENTLEER_Tick();
bool isAnTurmUnten();
bool isAnTurmOben();
bool isAnTurmBelegt();
bool isGreifarmOben();
bool isGreifarmUnten();
