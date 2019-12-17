#include "global.h"

void FS_Servo_init() {
	CAN_NMTConnect();
    for(int i = 0; i< 100000; i++);
    CAN_NMTConnect();
    for(int i = 0; i< 100000; i++);
    CAN_NMTConnect();

	FS_SERVO_timerDone = false;
	FS_SERVO_setTimer = true;
	step = FS_SERVO_INITIAL_MOVE;
	counterA = 0;

	direction = MotorStop;
	prevDirection = 3;

	conveyorDirection = conveyorStop;
	conveyorDirectionPrev = 3;

	CAN_ClearFlag(CAN1, CAN_FLAG_FF1);
	CAN_ClearFlag(CAN1, CAN_FLAG_FOV1);
}

bool FS_SERVO_BeroActive() { return (FS_SERVO_InData[0] & FS_SERVO_BERO_MASK) ? true : false; }
bool FS_SERVO_IsRefPressed() { return (FS_SERVO_InData[0] & FS_SERVO_REF_MASK) ? true : false; }
bool FS_SERVO_IncAChanged() { return (FS_SERVO_InData[0] & FS_SERVO_INC_A_MASK) ? true : false; }

void FS_SERVO_NewData(CanRxMsg RxMessage) {
	if (RxMessage.DLC == CAN_DLC_1) {
		FS_SERVO_InData[0] = RxMessage.Data[0];
	}
    if (FS_SERVO_IncAChanged()) {
		counterA++;
    }
}

void FS_SERVO_controlMotor(uint8_t direction) {
	uint8_t dataToSend[CAN_DLC_2];

	switch (direction) {
        case MoveToEmptying:
            dataToSend[0] = (uint8_t)0x00;
            dataToSend[1] = (uint8_t)0x70;
            break;

        case MoveToPicking:
            dataToSend[0] = (uint8_t)0x00;
            dataToSend[1] = (uint8_t)-0x70;
            break;

        case MotorStop:
            dataToSend[0] = (uint8_t)0x00;
            dataToSend[1] = (uint8_t)0x00;
            break;

        default: break;
	}
	CAN_TransmitMsg(FS_SERVO_AO_11Bit_ID, dataToSend, CAN_DLC_2);
}

void FS_SERVO_conveyorMovement(uint8_t direction) {
	uint8_t dataToSend[CAN_DLC_1];

	switch (direction) {
        case conveyorStop:
            dataToSend[0] = FS_SERVO_CONVEYOR_STOP;
            break;

        case conveyorRight:
            dataToSend[0] = FS_SERVO_CONVEYOR_RIGHT;
            break;

        case conveyorLeft:
            dataToSend[0] = FS_SERVO_CONVEYOR_LEFT;
            break;

        default: break;
	}
	CAN_TransmitMsg(FS_SERVO_DO_11Bit_ID, dataToSend, CAN_DLC_1);
}

void FS_SERVO_Routine() {

    switch (step) {
        case FS_SERVO_INITIAL_MOVE:
            TIM_Cmd(TIM4, ENABLE);
            conveyorDirection = conveyorStop;
            direction = MoveToEmptying;
            if (FS_SERVO_timerDone) {
                TIM_SetAutoreload(TIM4, FS_SERVO_TIMER_ARR);
                step = FS_SERVO_DRIVE_TO_REFKEY;
            }
            break;

        case FS_SERVO_DRIVE_TO_REFKEY:
            conveyorDirection = conveyorStop;
            if (FS_SERVO_IsRefPressed()) {
                direction = MotorStop;
                counterA = 0;
                step = FS_SERVO_DRIVE_TO_PICKING;
            }
            else {
                direction = MoveToPicking;
                counterA = 0;
            }
            break;

        case FS_SERVO_DRIVE_TO_PICKING:
            direction = MoveToPicking;
            if (counterA == FS_SERVO_STEPS_TO_REF) {
                counterA = 0;
                direction = MotorStop;
                step = FS_SERVO_WAITING_FOR_BLOCK;
            }
            break;

        case FS_SERVO_WAITING_FOR_BLOCK:
            FS_SERVO_arrived = true;
            direction = MotorStop;
            if (FS_KOM_Ready_to_give_Block) {
                FS_SERVO_arrived = false;
                step = FS_SERVO_TAKE_BLOCK;
            }
            break;

        case FS_SERVO_TAKE_BLOCK:
            if (FS_SERVO_setTimer && FS_KOM_Ready_to_give_Block) {
                FS_SERVO_setTimer = false;
                FS_SERVO_timerDone = false;
                TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
                TIM_Cmd(TIM4, ENABLE);
                FS_KOM_Ready_to_give_Block = false;
            }
            conveyorDirection = conveyorRight;
            if (FS_SERVO_BeroActive()) {
                conveyorDirection = conveyorStop;
                counterA = 0;
                step = FS_SERVO_DRIVE_TO_EMPTYING;
            }
            if (FS_SERVO_timerDone) {
                FS_SERVO_timerDone = false;
                conveyorDirection = conveyorStop;
                FS_SERVO_setTimer = true;
                step = FS_SERVO_WAITING_FOR_BLOCK;
            }
            break;

        case FS_SERVO_DRIVE_TO_EMPTYING:
            conveyorDirection = conveyorStop;
            FS_KOM_Ready_to_give_Block = false;
            direction = MoveToEmptying;
            if (counterA == FS_SERVO_STEPS_TO_EMPTYING) {
                counterA = 0;
                direction = MotorStop;
                step = FS_SERVO_STOP_AT_EMPTYING;
            }
            if (!FS_SERVO_BeroActive()) {
                step = FS_SERVO_DRIVE_TO_REFKEY;
            }
            break;

        case FS_SERVO_STOP_AT_EMPTYING:
            direction = MotorStop;
            FS_SERVO_timerDone = false;
            FS_SERVO_setTimer = true;
            step = FS_SERVO_WAIT_FOR_EMPTYING;
            break;

        case FS_SERVO_WAIT_FOR_EMPTYING:
            if (FS_ENTLEER_ANTURM_ANNAHMEBEREIT) {
                conveyorDirection = conveyorLeft;
                if(!FS_SERVO_blockReady)
                    FS_ENTLEER_ExtraTickRequest = true;

                FS_SERVO_blockReady = true;
            }
            if (FS_ENTLEER_ANTURM_BLOCK_ERHALTEN) {
                FS_SERVO_blockReady = false;
                conveyorDirection = conveyorStop;
                step = FS_SERVO_STOP_CONVEYOR;
            }
            break;

        case FS_SERVO_STOP_CONVEYOR:
            conveyorDirection = conveyorStop;
            if (!FS_SERVO_BeroActive()) {
                conveyorDirection = conveyorStop;
                step = FS_SERVO_DRIVE_TO_REFKEY;
            }
            break;
        default: break;
	}

	//Senden des Telegrams zur Ansteuerung des Servormotors, wenn sich die Servomotorrichtung ändert
	if (direction != prevDirection) {
		prevDirection = direction;
		FS_SERVO_controlMotor(direction);
	}

	//Senden des Telegrams zur Fließbandansteuerung, wenn sich der Fließbandzustand ändern soll
	if (conveyorDirection != conveyorDirectionPrev) {
		conveyorDirectionPrev = conveyorDirection;
		FS_SERVO_conveyorMovement(conveyorDirection);
        }
}
