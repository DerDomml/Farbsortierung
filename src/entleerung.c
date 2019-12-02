#include "global.h"

void testStartFliessband() {
    uint8_t data = 0x8;
    CAN_TransmitMsg(0x213, &data, 1);
}

void testStopFliessband() {
    uint8_t data = 0x0;
    CAN_TransmitMsg(0x213, &data, 1);
}
