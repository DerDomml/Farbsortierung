/****************************************************
	Initialization of Ports, LCD, Timer and ...
****************************************************/
#include "global.h"
//#include "ILI9341.h"

void PortsInit(void)
// Recommendation: Switch DILs to pull down when used as input
/*
    Unusable Port Bits:
    A15 A14 A13 A12    A11      A8  A7            A2 A1
    B15 B14 B13 B12    B11 B10            B4   B3
    C15 C14     C12    C11 C10         C5 C4         C1
*/
{
    //CAN Ports init
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIOInit;
    GPIOInit.GPIO_Speed = GPIO_Speed_50MHz;
    GPIOInit.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIOInit.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC,&GPIOInit);

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIOInit.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_Init(GPIOE, &GPIOInit);

    GPIOInit.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIOInit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOE, &GPIOInit);
}

void TimerInit(void)
{
    //CAN Clock init
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);

    TIM_TimeBaseInitTypeDef TimInit;
    TIM_TimeBaseStructInit(&TimInit);
    TimInit.TIM_Period = 60000 - 1;
    TimInit.TIM_Prescaler = 1200-1;
    TIM_TimeBaseInit(TIM6,&TimInit);
    TIM_ClearITPendingBit(TIM6,TIM_IT_Update);
    TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM6,ENABLE);

}

void IntInit()
{
    /*
    General Configuration of Priority Group
    */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    /*
            Timer Interrupts
    */


    NVIC_InitTypeDef Interrupt;
    Interrupt.NVIC_IRQChannel = TIM6_IRQn;
    Interrupt.NVIC_IRQChannelCmd = ENABLE;
    Interrupt.NVIC_IRQChannelPreemptionPriority = 5;
    Interrupt.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&Interrupt);
    TIM_ClearITPendingBit(TIM6,TIM_IT_Update);

    TIM_Cmd(TIM6, ENABLE);

}

void ProjectInit(void)
{
    PortsInit();
    TimerInit();
    IntInit();
    CAN_Init4Models();
}


