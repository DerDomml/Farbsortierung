/****************************************************
	Kommissionierungs Programm
****************************************************/

#include "global.h"

static uint8_t FS_KOM_DI_INData[FS_KOM_DATALength];

static uint8_t Farbe12;
static uint8_t Farbe22;
static uint8_t Farbe32;

static bool TransitionFill1toMoveto2 = false;
static bool FirstScan = true;

//Initialize "Kommissionierung"
void FS_KOM_Init(){

    //Reset all Actors
    uint8_t Start[] = {0x00,0x00};
    FS_KOM_ENABLE_DO_VerMag1(Start);
    FS_KOM_ENABLE_DO_VerMag2(Start);
    FS_KOM_ENABLE_DO_VerMag3(Start);
    CAN_TransmitMsg(FS_KOM_DO_11Bit_ID,Start,CAN_DLC_2);

    //Set Default Step
    FS_KOM_Schritt = FS_KOM_Step_AddBlock;


    //Set Default State of Global Variables
    FS_KOM_Block_Waiting = false;
    FS_KOM_NEWTelegram = false;
    FS_KOM_Ready_to_take_Block = true;
    FS_KOM_Ready_to_give_Block= false;
    TransitionFill1toMoveto2 = false;

    TIM_Cmd(TIM2,ENABLE);
}

//Receive new Data
void FS_KOM_NewData(CanRxMsg RxMessage){

    //Receive new Data
    if(RxMessage.DLC == CAN_DLC_2)
        for(int i = 0; i < FS_KOM_DATALength;i++)
            FS_KOM_DI_INData[i] = RxMessage.Data[i];

    //Set New Telegram Notification
    FS_KOM_NEWTelegram = true;
}

//Routine for "Kommissionierung"
void FS_KOM_Routine(){

    //Static Routine Variables

    //Break at Blockloss
    static uint8_t Overflow = 0;

    //Variables for Order
    static uint8_t Zahl = 0;
    static uint8_t Farbe1 = 0;
    static uint8_t Farbe2 = 0;
    static uint8_t Farbe3 = 0;

    Farbe12 = Farbe1;
    Farbe22 = Farbe2;
    Farbe32 = Farbe3;

    //Reed Trigger Sensitivity
    bool REED1_prev = false;
    bool REED2_prev = false;

    //Declare Output Data
    uint8_t FS_KOM_DO_OUTData[] = {0x00,0x00};

    //Reset New Telegram Notification
    if(FS_KOM_Timer && FirstScan){
        FirstScan = false;
        FS_KOM_Timer =false;
    }

    if(FirstScan)
        return;

    //FS_KOM_OrderBalls();

    if(FS_KOM_NEWTelegram)
        FS_KOM_NEWTelegram = false;

    //Reset Block Waiting when it starts moving
    if(FS_KOM_Block_Waiting && FS_KOM_Schritt == FS_KOM_Step_AddBlock)
        FS_KOM_Block_Waiting = false;

    //Get Order Information in Reed1
    if(FS_KOM_CHECK_DI_REED1() && !REED1_prev){
        Zahl+=10;
        REED1_prev = true;
    }else
        REED1_prev = false;

    //Get Order Information in Reed2
    if(FS_KOM_CHECK_DI_REED2() && !REED2_prev){
        Zahl+=1;
        REED2_prev = true;
    }else
        REED2_prev = false;

    //Receive Second Block while working on Order
    if(FS_KOM_Schritt > FS_KOM_Step_MoveToBero1){
        //Generate Time and set register Blockloss at 10 seconds
        if(FS_KOM_Timer && FS_Vorgaenger)//---------------------------------------Vorgaenger!!!
            Overflow++;

        if(FS_Vorgaenger && !FS_KOM_CHECK_DI_BERO0() && Overflow < 10){//---------------------------------------Vorgaenger!!!
            FS_KOM_ENABLE_DO_IBAND_Xplus(FS_KOM_DO_OUTData);
        }else{
            FS_Vorgaenger = false;//---------------------------------------Vorgaenger!!!
            FS_KOM_Block_Waiting = true;
            Overflow = 0;
        }
    }

    //Generate Number of Colors by Reed Input
    if(FS_KOM_Schritt == FS_KOM_Step_MoveToBero1 && !FS_KOM_CHECK_DI_BERO0()){
        if(!FS_SORT_GetCurrentAmountOfOrderedBalls(&Farbe1,&Farbe2,&Farbe3))
            FS_KOM_Auftrag(Zahl, &Farbe1, &Farbe2, &Farbe3);
    }

    static bool Zahlprev = false;
    //Reset Reed Input
    if(FS_KOM_Schritt == FS_KOM_Step_MoveToBero1 && !Zahlprev){
        Zahlprev = true;
        Zahl = 0;
    }

    if(FS_KOM_Schritt == FS_KOM_Step_FillColor1 && Zahlprev)
        Zahlprev = false;

    //Set Step and check Transitions
    FS_KOM_Schritt = FS_KOM_Schrittwahl(&Farbe1,&Farbe2,&Farbe3);

    //Write Data According to Step
    FS_KOM_WRITE_Data(FS_KOM_DO_OUTData,&FS_KOM_Schritt,&Farbe1, &Farbe2, &Farbe3);

    //Send Data
    CAN_TransmitMsg(FS_KOM_DO_11Bit_ID,FS_KOM_DO_OUTData,CAN_DLC_2);
}

//Set Step and check Transitions
uint8_t FS_KOM_Schrittwahl(uint8_t *Farbe1, uint8_t *Farbe2, uint8_t *Farbe3){

    //Movement Violation Timer
    static uint8_t Overflow = 0;

    switch(FS_KOM_Schritt){
        //------------------------------------//
        case FS_KOM_Step_AddBlock:
            //Check Transition
            if(FS_KOM_CHECK_DI_BERO0())
                return FS_KOM_Step_MoveToBero1;
            break;

        //------------------------------------//
        case FS_KOM_Step_MoveToBero1:
            //Enable Timer for Movement Violation
            TIM_Cmd(TIM2,ENABLE);
            //Checking for Movement Violation
            if(FS_KOM_Timer == true){
                Overflow++;
                FS_KOM_Timer = false;
            }
            if(Overflow == 10){
                TIM_Cmd(TIM2,DISABLE);
                FS_KOM_Timer = false;
                Overflow = 0;
                return FS_KOM_Step_ResetVerMag;
            }
            //Check Transition
            if(FS_KOM_CHECK_DI_BERO1()){
                return FS_KOM_Step_FillColor1;
            }
            break;

        //------------------------------------//
        case FS_KOM_Step_FillColor1:
            Overflow = 0;
            //Check Transition
            if(TransitionFill1toMoveto2 && *Farbe1 == 0)
                return FS_KOM_Step_MoveToBero2;
            break;

        //------------------------------------//
        case FS_KOM_Step_MoveToBero2:
            //Enable Timer for Movement Violation
            TIM_Cmd(TIM2,ENABLE);
            //Checking for Movement Violation
            if(FS_KOM_Timer == true){
                Overflow++;
                FS_KOM_Timer = false;
            }
            //Reset to First Step in Case of Movement Violation
            if(Overflow == 10){
                TIM_Cmd(TIM2,DISABLE);
                FS_KOM_Timer = false;
                Overflow = 0;
                return FS_KOM_Step_AddBlock;
            }
            //----------------------------------------------------------FS_KOM_Timer = false;
            //Check Transition
            if(FS_KOM_CHECK_DI_BERO2()){
                if(*Farbe2 == 0)
                    return FS_KOM_Step_MoveToBero3;
                return FS_KOM_Step_FillColor2;
            }
            break;

        //------------------------------------//
        case FS_KOM_Step_FillColor2:
            //Reset Violation Timer
            Overflow = 0;
            //Check Transition
            if(*Farbe2 == 0)
                return FS_KOM_Step_MoveToBero3;
            break;

        //------------------------------------//
        case FS_KOM_Step_MoveToBero3:
            //Enable Timer for Movement Violation
            TIM_Cmd(TIM2,ENABLE);
            //Checking for Movement Violation
            if(FS_KOM_Timer == true){
                    Overflow++;
                    FS_KOM_Timer = false;
            }
            //Reset to First Step in Case of Movement Violation
            if(Overflow == 10){
                TIM_Cmd(TIM2,DISABLE);
                FS_KOM_Timer = false;
                Overflow = 0;
                return FS_KOM_Step_AddBlock;
            }
            //------------------------------------------------------------FS_KOM_Timer = false;
            //Check Transition
            if(FS_KOM_CHECK_DI_BERO3())
                return FS_KOM_Step_FillColor3;
            break;

        //------------------------------------//
        case FS_KOM_Step_FillColor3:
            //Reset Violation Timer
            Overflow = 0;
            //Check Transition
            if(*Farbe3 == 0)
                return FS_KOM_Step_WaitForServo;
            break;

        //------------------------------------//
        case FS_KOM_Step_WaitForServo:
            FS_KOM_Timer = false;
            //Check Transition
            if(FS_SERVO_arrived)
                return FS_KOM_Step_MoveToServo;
            break;

        //------------------------------------//
        case FS_KOM_Step_MoveToServo:
            //Enable Timer for Transportation
            TIM_Cmd(TIM2,ENABLE);
            //Define Timer for Transportation
            static uint8_t Wait = 0;
            //Increment Timer
            if(FS_KOM_Timer == true){
                Wait++;
                FS_KOM_Timer = false;
            }
            //Transition to First Step after 3 seconds of delivering
            if(Wait == 3){
                TIM_Cmd(TIM2,DISABLE);
                FS_KOM_Timer = false;
                Wait = 0;
                if(FS_KOM_Block_Waiting){
                    FS_KOM_NEWTelegram = true;
                }
                return FS_KOM_Step_AddBlock;
            }
            break;

        //------------------------------------//
        case FS_KOM_Step_ResetVerMag:
            //Enable Timer for Magazine Reset
            TIM_Cmd(TIM2,ENABLE);
            //Set VerMag for 2 seconds than return to First Step
            if(FS_KOM_Timer == true){
                Wait++;
                FS_KOM_Timer = false;
            }
            if(Wait == 2){
                TIM_Cmd(TIM2,DISABLE);
                Wait = 0;
                return FS_KOM_Step_AddBlock;
            }
            break;
    }
    return FS_KOM_Schritt;
}

//Write Data According to Step
void FS_KOM_WRITE_Data(uint8_t *FS_KOM_DO_OUTData, uint8_t *FS_KOM_Schritt,uint8_t *Farbe1, uint8_t *Farbe2, uint8_t *Farbe3){
    //Define bool for Timer Trigger
    static bool FS_KOM_Timer_prev = false;
    static bool ColorsChecked = false;

    switch(*FS_KOM_Schritt){

        //------------------------------------//
        case FS_KOM_Step_AddBlock:
            if(!FS_KOM_Block_Waiting)
            FS_KOM_Ready_to_take_Block = true;
            //Move New Block to Bero0
            if((FS_Vorgaenger||FS_KOM_CHECK_DI_REED1()||FS_KOM_CHECK_DI_REED2())&&FS_KOM_Ready_to_take_Block){//---------------------------------------Vorgaenger!!!
                FS_KOM_ENABLE_DO_IBAND_Xplus(FS_KOM_DO_OUTData);
                FS_Vorgaenger = false;//-------------------------------------------------Voragaenger!!!!!!!
            }
            break;

        //------------------------------------//
        case FS_KOM_Step_MoveToBero1:
            //Move Block to Bero1
            FS_KOM_Ready_to_take_Block = false;
            FS_KOM_ENABLE_DO_IBAND_Xplus(FS_KOM_DO_OUTData);
            FS_KOM_ENABLE_DO_BAND_Xplus(FS_KOM_DO_OUTData);
            break;

        //------------------------------------//
        case FS_KOM_Step_FillColor1:
            //Enable Timer for filling Block
            TIM_Cmd(TIM2,ENABLE);

            if(FS_SORT_GetAmount(FS_SORT_ColorOrange) >= *Farbe1 && FS_SORT_GetAmount(FS_SORT_ColorBlue) >= *Farbe2 && FS_SORT_GetAmount(FS_SORT_ColorGreen) >= *Farbe3){
                FS_SORT_ZYLINDER_start = false;
                FS_ENTLEER_ExtraTickRequest = true;
                if(!ColorsChecked){
                        uint8_t CheckF1;
                        uint8_t CheckF2;
                        uint8_t CheckF3;
                        FS_SORT_GetCurrentAmountOfOrderedBalls(&CheckF1,&CheckF2,&CheckF3);
                        if(CheckF1 == *Farbe1 && CheckF2 == *Farbe2 && CheckF3 == *Farbe3)
                            FS_SORT_UpdateArray();
                        ColorsChecked = true;
                }

                //Check if Block gets removed or falls down
                if(FS_KOM_CHECK_DI_BERO1()){
                    //Reverse the KuFrMag and the VerMag as often as Color is ordered
                    if(*Farbe1 != 0){
                        if(FS_KOM_Timer && !FS_KOM_Timer_prev){
                            FS_KOM_ENABLE_DO_KuFrMag1(FS_KOM_DO_OUTData);
                            FS_KOM_Timer_prev = true;
                            FS_KOM_Timer = false;
                            (*Farbe1)--;
                            FS_SORT_RemoveBall(FS_SORT_ColorOrange);
                        }else if(FS_KOM_Timer && FS_KOM_Timer_prev){
                            FS_KOM_ENABLE_DO_VerMag1(FS_KOM_DO_OUTData);
                            FS_KOM_Timer = false;
                            FS_KOM_Timer_prev = false;
                        }
                    }else{
                        FS_KOM_ENABLE_DO_VerMag1(FS_KOM_DO_OUTData);

                        if(FS_KOM_Timer){
                            FS_KOM_Timer = false;
                            FS_KOM_Timer_prev = false;
                            TransitionFill1toMoveto2 = true;
                        }
                    }

                //Set VerMag and Reset KuFraMag if Block is Lost and Return to First Step
                }else{
                    FS_KOM_Timer_prev = false;
                    *FS_KOM_Schritt = FS_KOM_Step_ResetVerMag;
                    FS_KOM_NEWTelegram = true;
                    FS_KOM_Timer = false;
                    TransitionFill1toMoveto2 = true;
                }
            }else{
                if(FS_SORT_GetAmount(FS_SORT_ColorOrange) >= FS_SORT_MaxBallsinMag || FS_SORT_GetAmount(FS_SORT_ColorBlue) >= FS_SORT_MaxBallsinMag || FS_SORT_GetAmount(FS_SORT_ColorGreen) >= FS_SORT_MaxBallsinMag){
                    if(FS_SORT_GetAmount(FS_SORT_ColorOrange) >= FS_SORT_MaxBallsinMag){
                        *Farbe1 = 6;
                        *Farbe2 = 0;
                        *Farbe3 = 0;
                    }
                    if(FS_SORT_GetAmount(FS_SORT_ColorBlue) >= FS_SORT_MaxBallsinMag){
                        *Farbe1 = 0;
                        *Farbe2 = 6;
                        *Farbe3 = 0;
                    }
                    if(FS_SORT_GetAmount(FS_SORT_ColorGreen) >= FS_SORT_MaxBallsinMag){
                        *Farbe1 = 0;
                        *Farbe2 = 0;
                        *Farbe3 = 6;
                    }
                }else{
                    FS_SORT_ZYLINDER_start = true;
                    FS_ENTLEER_ExtraTickRequest = true;
                    //NEEED BAAAAAAAAAAAAAAAAAAAAAAAAAAAAALLS
                }
                FS_KOM_Timer = false;
            }

            break;

        //------------------------------------//
        case FS_KOM_Step_MoveToBero2:
            TransitionFill1toMoveto2 = false;
            ColorsChecked = false;
            //Move Block to Bero2
            FS_KOM_ENABLE_DO_BAND_Xplus(FS_KOM_DO_OUTData);
            break;

        //------------------------------------//
        case FS_KOM_Step_FillColor2:
            //Enable Timer for filling Block
            TIM_Cmd(TIM2,ENABLE);
            //Check if Block gets removed or falls down
            if(FS_KOM_CHECK_DI_BERO2()){
                //Reverse the KuFrMag and the VerMag as often as Color is ordered
                if(FS_KOM_Timer && !FS_KOM_Timer_prev){
                    FS_KOM_ENABLE_DO_KuFrMag2(FS_KOM_DO_OUTData);
                    FS_KOM_Timer_prev = true;
                    FS_SORT_RemoveBall(FS_SORT_ColorBlue);
                    FS_KOM_Timer = false;
                }else if(FS_KOM_Timer && FS_KOM_Timer_prev){
                    FS_KOM_ENABLE_DO_VerMag2(FS_KOM_DO_OUTData);
                    FS_KOM_Timer_prev = false;
                    FS_KOM_Timer = false;
                    (*Farbe2)--;
                }
            //Set VerMag and Reset KuFraMag if Block is Lost and Return to First Step
            }else{
                FS_KOM_Timer_prev = false;
                FS_KOM_Timer = false;
                *FS_KOM_Schritt = FS_KOM_Step_ResetVerMag;
                TIM_Cmd(TIM2,DISABLE);
                FS_KOM_NEWTelegram = true;
            }
            break;

        //------------------------------------//
        case FS_KOM_Step_MoveToBero3:
            FS_KOM_ENABLE_DO_BAND_Xplus(FS_KOM_DO_OUTData);
            break;

        //------------------------------------//
        case FS_KOM_Step_FillColor3:
            //Enable Timer for filling Block
            TIM_Cmd(TIM2,ENABLE);
            //Check if Block gets removed or falls down
            if(FS_KOM_CHECK_DI_BERO3()){
                //Reverse the KuFrMag and the VerMag as often as Color is ordered
                if(FS_KOM_Timer && !FS_KOM_Timer_prev){
                    FS_KOM_ENABLE_DO_KuFrMag3(FS_KOM_DO_OUTData);
                    FS_KOM_Timer_prev = true;
                    FS_SORT_RemoveBall(FS_SORT_ColorGreen);
                    FS_KOM_Timer = false;
                }else if(FS_KOM_Timer && FS_KOM_Timer_prev){
                    FS_KOM_ENABLE_DO_VerMag3(FS_KOM_DO_OUTData);
                    FS_KOM_Timer_prev = false;
                    FS_KOM_Timer = false;
                    (*Farbe3)--;
                }
            //Set VerMag and Reset KuFraMag if Block is Lost and Return to First Step
            }else{
                FS_KOM_Timer_prev = false;
                FS_KOM_Timer = false;
                *FS_KOM_Schritt = FS_KOM_Step_ResetVerMag;
                TIM_Cmd(TIM2,DISABLE);
                FS_KOM_NEWTelegram = true;
            }
            break;

        //------------------------------------//
        case FS_KOM_Step_WaitForServo:
            //Set Ready to Give Block
            //Check if Block gets removed or falls down
            if(!FS_KOM_CHECK_DI_BERO3()){
                FS_KOM_Ready_to_give_Block = false;
                *FS_KOM_Schritt = FS_KOM_Step_AddBlock;
            }
            break;

        //------------------------------------//
        case FS_KOM_Step_MoveToServo:
            //Move Block to Servo
            FS_KOM_Ready_to_give_Block = true;
            FS_KOM_ENABLE_DO_BAND_Xplus(FS_KOM_DO_OUTData);
            break;

        //------------------------------------//
        case FS_KOM_Step_ResetVerMag:
            //Set VerMag and Reset KuFraMag for Restart
            FS_KOM_ENABLE_DO_VerMag1(FS_KOM_DO_OUTData);
            FS_KOM_ENABLE_DO_VerMag2(FS_KOM_DO_OUTData);
            FS_KOM_ENABLE_DO_VerMag3(FS_KOM_DO_OUTData);
            break;
    }
}

//Generate Number of Colors by Reed Input
void FS_KOM_Auftrag(uint8_t Zahl, uint8_t *Farbe1,uint8_t *Farbe2,uint8_t *Farbe3){

    switch(Zahl){
        case 0:break;
        case 1: *Farbe1 = 0;*Farbe2 = 1;*Farbe3 = 1;break;
        case 2: *Farbe1 = 0;*Farbe2 = 3;*Farbe3 = 1;break;
        case 10: *Farbe1 = 0;*Farbe2 = 1;*Farbe3 = 1;break;
        case 11: *Farbe1 = 1;*Farbe2 = 1;*Farbe3 = 1;break;
        case 12: *Farbe1 = 1;*Farbe2 = 1;*Farbe3 = 1;break;
        case 20: *Farbe1 = 0;*Farbe2 = 2;*Farbe3 = 3;break;
        case 21: *Farbe1 = 1;*Farbe2 = 1;*Farbe3 = 1;break;
        case 22: *Farbe1 = 0;*Farbe2 = 1;*Farbe3 = 1;break;
        default: *Farbe1 = 2;*Farbe2 = 2;*Farbe3 = 2;break;
    }
}

//Check if Routine should be Accessed
bool FS_KOM_CHECK_RoutineCondition(){
    //Previous Trigger
    static bool prev = false;
    //Access Routine with new Telegram
    if(FS_KOM_NEWTelegram)
        return true;
    //Access Routine with Timer Event
    if(FS_KOM_Timer)
        return true;
    //Access Routine with Already Waiting Block at Start
    if(FS_KOM_Schritt == FS_KOM_Step_AddBlock && FS_KOM_Block_Waiting)
        return true;
    //Access Routine if Servo is Ready
    if(FS_KOM_Schritt == FS_KOM_Step_WaitForServo && FS_SERVO_arrived)
        return true;
    //Access Routine to input New Block while waiting for Servo
    if(FS_KOM_Schritt == FS_KOM_Step_WaitForServo && FS_Vorgaenger && !prev){//---------------------------------------Vorgaenger!!!
        prev = true;
        return true;
    }
    //Reset Previous Trigger
    if(!FS_Vorgaenger && prev)//---------------------------------------Vorgaenger!!!
        prev = false;
    //Access Routine with new Block from StepMotor
    if(FS_KOM_Schritt == FS_KOM_Step_AddBlock && FS_Vorgaenger)//---------------------------------------Vorgaenger!!!
        return true;

    return false;
}

void FS_KOM_OrderBalls(){
        static bool Ballsprev = false;
     if(FS_KOM_Timer&&(FS_SORT_GetAmount(FS_SORT_ColorOrange) <=6 || FS_SORT_GetAmount(FS_SORT_ColorBlue) <=6|| FS_SORT_GetAmount(FS_SORT_ColorGreen) <=6)&&FS_SORT_GetAmount(FS_SORT_ColorOrange) < FS_SORT_MaxBallsinMag&&FS_SORT_GetAmount(FS_SORT_ColorBlue) < FS_SORT_MaxBallsinMag&&FS_SORT_GetAmount(FS_SORT_ColorGreen) < FS_SORT_MaxBallsinMag){
        FS_SORT_ZYLINDER_start = true;
        FS_ENTLEER_ExtraTickRequest = true;
        Ballsprev = true;
     }else{
         if(Ballsprev){
            FS_SORT_ZYLINDER_start = false;
            FS_ENTLEER_ExtraTickRequest = true;
            Ballsprev = false;
         }
     }
}

//Check Sensors

//FS_KOM_DI_LT1

bool FS_KOM_CHECK_DI_LT1(){

    if(FS_KOM_DI_INData[0] & FS_KOM_DI_LT1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FS_KOM_DI_LT2

bool FS_KOM_CHECK_DI_LT2(){

    if(FS_KOM_DI_INData[0] & FS_KOM_DI_LT2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FS_KOM_DI_LT3

bool FS_KOM_CHECK_DI_LT3(){

    if(FS_KOM_DI_INData[1] & FS_KOM_DI_LT3)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FS_KOM_DI_BERO1

bool FS_KOM_CHECK_DI_BERO1(){

    if(FS_KOM_DI_INData[0] & FS_KOM_DI_BERO1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FS_KOM_DI_BERO2

bool FS_KOM_CHECK_DI_BERO2(){

    if(FS_KOM_DI_INData[0] & FS_KOM_DI_BERO2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FS_KOM_DI_BERO3

bool FS_KOM_CHECK_DI_BERO3(){

    if(FS_KOM_DI_INData[0] & FS_KOM_DI_BERO3)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FS_KOM_DI_BERO0

bool FS_KOM_CHECK_DI_BERO0(){

    if(FS_KOM_DI_INData[0] & FS_KOM_DI_BERO0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FS_KOM_DI_REED1

bool FS_KOM_CHECK_DI_REED1(){

    if(FS_KOM_DI_INData[0] & FS_KOM_DI_REED1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//FS_KOM_DI_REED2

bool FS_KOM_CHECK_DI_REED2(){

    if(FS_KOM_DI_INData[0] & FS_KOM_DI_REED2)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//Activate Actor

//FS_KOM_DO_KuFrMag3

void FS_KOM_ENABLE_DO_KuFrMag3(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] |= FS_KOM_DO_KuFrMag3;
}

void FS_KOM_DISABLE_DO_KuFrMag3(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] &= ~FS_KOM_DO_KuFrMag3;
}

//FS_KOM_DO_KuFrMag2

void FS_KOM_ENABLE_DO_KuFrMag2(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] |= FS_KOM_DO_KuFrMag2;
}

void FS_KOM_DISABLE_DO_KuFrMag2(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] &= ~FS_KOM_DO_KuFrMag2;
}

//FS_KOM_DO_KuFrMag1

void FS_KOM_ENABLE_DO_KuFrMag1(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] |= FS_KOM_DO_KuFrMag1;
}

void FS_KOM_DISABLE_DO_KuFrMag1(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] &= ~FS_KOM_DO_KuFrMag1;
}

//FS_KOM_DO_VerMag3

void FS_KOM_ENABLE_DO_VerMag3(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[1] |= FS_KOM_DO_VerMag3;
}

void FS_KOM_DISABLE_DO_VerMag3(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[1] &= ~FS_KOM_DO_VerMag3;
}

//FS_KOM_DO_VerMag2

void FS_KOM_ENABLE_DO_VerMag2(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] |= FS_KOM_DO_VerMag2;
}

void FS_KOM_DISABLE_DO_VerMag2(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] &= ~FS_KOM_DO_VerMag2;
}

//FS_KOM_DO_VerMag1

void FS_KOM_ENABLE_DO_VerMag1(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] |= FS_KOM_DO_VerMag1;
}

void FS_KOM_DISABLE_DO_VerMag1(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] &= ~FS_KOM_DO_VerMag1;
}

//FS_KOM_DO_BAND_Xplus

void FS_KOM_ENABLE_DO_BAND_Xplus(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] |= FS_KOM_DO_BAND_Xplus;
}

void FS_KOM_DISABLE_DO_BAND_Xplus(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] &= ~FS_KOM_DO_BAND_Xplus;
}

//FS_KOM_DO_BAND_Xminus

void FS_KOM_ENABLE_DO_BAND_Xminus(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] |= FS_KOM_DO_BAND_Xminus;
}

void FS_KOM_DISABLE_DO_BAND_Xminus(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] &= ~FS_KOM_DO_BAND_Xminus;
}


//FS_KOM_DO_IBAND_Xplus

void FS_KOM_ENABLE_DO_IBAND_Xplus(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] |= FS_KOM_DO_IBAND_Xplus;
}

void FS_KOM_DISABLE_DO_IBAND_Xplus(uint8_t *FS_KOM_DO_OUTData){
    FS_KOM_DO_OUTData[0] &= ~FS_KOM_DO_IBAND_Xplus;
}
