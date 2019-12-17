/****************************************************
	ISRs - Interrupt Handlers
****************************************************/
#include "global.h"

uint16_t FS_SORT_UartRxData;

void TIM2_IRQHandler()
{
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
    //Set Timer Event
    FS_KOM_Timer = true;
}

void TIM3_IRQHandler()
{
    TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    FS_SORT_CloseStangen();
    FS_SORT_DeactivateLight();
}

void TIM4_IRQHandler(){

    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    FS_SERVO_timerDone = true;
    TIM_Cmd(TIM4, DISABLE);
}

void TIM6_IRQHandler(){

    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
//    FS_ENTLEER_WERKSTUECKVERLUST = true;
    TIM_Cmd(TIM6, DISABLE);
//    FS_ENTLEER_ExtraTickRequest = true;
}

void USART1_IRQHandler()
{
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    FS_SORT_UartRxData = USART_ReceiveData(USART1);

    if(FS_SORT_isOrder) {
        FS_SORT_GetOrder(FS_SORT_UartRxData);
    }
    else if(!FS_SORT_isOrder) {
        FS_SORT_SetStartAmount(FS_SORT_UartRxData);
    }
}

void EXTI15_10_IRQHandler()
{
    if(EXTI_GetITStatus(EXTI_Line10) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
        FS_Vorgaenger = true;
    }

    if(EXTI_GetITStatus(EXTI_Line11) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line11);
    }

    if(EXTI_GetITStatus(EXTI_Line14) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line14);
    }

    if(EXTI_GetITStatus(EXTI_Line15) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}

