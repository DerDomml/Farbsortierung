/****************************************************
	ISRs - Interrupt Handlers
****************************************************/
#include "global.h"


void EXTI15_10_IRQHandler()
{
    if (EXTI_GetFlagStatus(EXTI_Line10) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if (EXTI_GetFlagStatus(EXTI_Line11) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    if (EXTI_GetFlagStatus(EXTI_Line12) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if (EXTI_GetFlagStatus(EXTI_Line13) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if (EXTI_GetFlagStatus(EXTI_Line14) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if (EXTI_GetFlagStatus(EXTI_Line15) != RESET)
    {
        EXTI_ClearITPendingBit(EXTI_Line15);
    }
}

