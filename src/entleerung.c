#include "global.h"

void testStartFliessband() {
    uint8_t data[] = {0x01, 0x00};
    CAN_TransmitMsg(0x213, data, 2);
}

void testStopFliessband() {
    uint8_t data[] = {0x00, 0x00};
    CAN_TransmitMsg(0x213, data, 2);
}
