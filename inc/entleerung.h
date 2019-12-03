
#define FS_ENTLEER_ABTURM_OBEN          0x0001
#define FS_ENTLEER_ABTURM_UNTEN         0x0002
#define FS_ENTLEER_ABTURM_SENSOR        0x0004
#define FS_ENTLEER_ANTURM_OBEN          0x0100
#define FS_ENTLEER_ANTURM_UNTEN         0x0200
#define FS_ENTLEER_ANTURM_SENSOR        0x0400
#define FS_ENTLEER_ENTLEERPOS_SENSOR    0x0800
#define FS_ENTLEER_BO_ENDE_SENSOR       0x1000
#define FS_ENTLEER_KRAN_OBEN            0x2000
#define FS_ENTLEER_KRAN_UNTEN           0x4000

uint16_t CAN_received;
uint16_t CAN_toSend;

void FS_ENTLEER_Tick();
