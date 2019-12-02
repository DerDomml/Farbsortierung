/*
**              Basic Driver for CAN Communication
**					with Staudinger Models,
**					based on CANOpen
**
**********************************************************************/

#include "global.h"


/*******************************************************************
*																   *
*		 Initialization of Simulation Model						   *
*																   *
********************************************************************/
void CAN_Init4Models()
{
    Data[0] = 0x00;
    /*******************************************

        Port and Clock Initialization

    ******************************************/
    RCC_APB1PeriphClockCmd(RCC_APB1ENR_CAN1EN, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD |RCC_APB2ENR_AFIOEN, ENABLE);

    // Init CanRx mapped to GPIOD_0
    GPIO_InitTypeDef input;
    input.GPIO_Speed = GPIO_Speed_50MHz;
    input.GPIO_Mode = GPIO_Mode_IPU;
    input.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOD,&input);

    // Init CanTx mapped to GPIOD_1
    GPIO_InitTypeDef output;
    output.GPIO_Speed = GPIO_Speed_50MHz;
    output.GPIO_Mode = GPIO_Mode_AF_PP;
    output.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOD,&output);

    GPIO_PinRemapConfig(GPIO_Remap2_CAN1 , ENABLE);

    /*******************************************

        Initialization of CAN1

    *******************************************/

    CAN_InitTypeDef CAN1_InitStructure;

    CAN_DeInit(CAN1);
    CAN_StructInit(&CAN1_InitStructure);

    /**************************************
        Init of
        MCR (Master Control Register)
        MSR (Master Status Register)
        BTR (Bit Timing Register)
    **************************************/

    CAN1_InitStructure.CAN_Prescaler = 2;
    CAN1_InitStructure.CAN_BS1 = CAN_BS1_12tq;
    CAN1_InitStructure.CAN_BS2 = CAN_BS2_5tq;
    CAN1_InitStructure.CAN_SJW = CAN_SJW_1tq;
    CAN1_InitStructure.CAN_ABOM = ENABLE;
    CAN1_InitStructure.CAN_AWUM = ENABLE;
    CAN1_InitStructure.CAN_TTCM = ENABLE;
    CAN1_InitStructure.CAN_NART = DISABLE;
    CAN1_InitStructure.CAN_RFLM = DISABLE;
    CAN1_InitStructure.CAN_TXFP = DISABLE;
    CAN1_InitStructure.CAN_Mode = CAN_Mode_Normal;

    if (CAN_Init(CAN1, &CAN1_InitStructure))
    {

        /**************************************
            Init of Filter 0
            FMR (Master Control Register)
            FAR1 (Master Status Register)
            FSR1 (Bit Timing Register)
            set to Identifier List Mode
            -> filtering according to ID and List
        **************************************/
        CAN_DBGFreeze(CAN1, DISABLE);

        uint16_t id = 0x181;

        CAN_FilterInitTypeDef CAN1_Filter;

        CAN1_Filter.CAN_FilterNumber = 0;
        CAN1_Filter.CAN_FilterFIFOAssignment = CAN_FIFO0;
        CAN1_Filter.CAN_FilterMode = CAN_FilterMode_IdList;
        CAN1_Filter.CAN_FilterScale = CAN_FilterScale_16bit;
        CAN1_Filter.CAN_FilterActivation = ENABLE;
        CAN1_Filter.CAN_FilterIdHigh = 0;
        CAN1_Filter.CAN_FilterIdLow = id << 5;

        CAN_ITConfig(CAN1,CAN_IT_FMP0,ENABLE);

        CAN_FilterInit(&CAN1_Filter);

    }
    else
    {
        TFT_TextPuts("Error with CAN-Init \n", &Font_11x18, 0xffffff, 0x000000);
        while(true);
    }
/***********************************************************

    Initialization of NVIC to enable Receive Interrupt

***********************************************************/


    /*
            External Interrupts
    */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    NVIC_InitTypeDef NVICcan1rx;
    NVICcan1rx.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVICcan1rx.NVIC_IRQChannelCmd = ENABLE;
    NVICcan1rx.NVIC_IRQChannelPreemptionPriority = 5;
    NVICcan1rx.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVICcan1rx);

    CAN_ClearITPendingBit(CAN1,CAN_IT_FMP0);
}
/*******************************************************************
*																   *
*		 Transmit CAN-Messages and CAN-Connect NMT  			   *
*																   *
********************************************************************/

uint8_t CAN_TransmitMsg(uint16_t canId, uint8_t *canData, uint8_t canDlc){
    CanTxMsg TxMessage;

    TxMessage.DLC = canDlc;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.StdId = canId;

    for(int i = 0; i < canDlc;i++){
        TxMessage.Data[i] = canData[i];
    }

    return CAN_Transmit(CAN1,&TxMessage);
}

uint8_t CAN_NMTConnect(){
    CanTxMsg TxMessage;
    uint32_t NMT_id = 0x415;

    TxMessage.DLC = CAN_DLC_2;
    TxMessage.IDE = CAN_Id_Standard;
    TxMessage.RTR = CAN_RTR_DATA;
    TxMessage.StdId = NMT_id;

    TxMessage.Data[0] = 0x01;
    TxMessage.Data[1] = 0x00;

    for(int i = 2; i < 8; i++){
        TxMessage.Data[i] = 0x00;
    }

    return CAN_Transmit(CAN1,&TxMessage);
}

/*******************************************************************
*																   *
*		 Interrupt Handler for CAN-Receive						   *
*																   *
********************************************************************/
// CAN cleared auto pending, PM 126,
void CAN1_RX0_IRQHandler(void)
{
    CanRxMsg RxMessage;
    CAN_ClearFlag(CAN1,CAN_IT_FMP0);
    CAN_Receive(CAN1,CAN_FIFO0, &RxMessage);
    memcpy(Data,RxMessage.Data,8);
    //Data = RxMessage.Data;
}


//
void CAN1_TXRQ_IRQHandler(void)
{
    CAN_ClearITPendingBit(CAN1,CAN_IT_RQCP0);
}
