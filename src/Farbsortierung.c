/****************************************************
	Projekt Farbsortierung
****************************************************/
#include "global.h"

//ServoMotor
void FS_SERVO_GpioInit(){
}

void FS_SERVO_TimerInit(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);

    TIM_TimeBaseInitTypeDef TimInit;
    TIM_TimeBaseStructInit(&TimInit);
    TimInit.TIM_Period = 10000 - 1;
    TimInit.TIM_Prescaler = 7200-1;
    TIM_TimeBaseInit(TIM4,&TimInit);
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);
    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM4,DISABLE);
}

void FS_SERVO_IntInit(){
    NVIC_InitTypeDef Interrupt;
    Interrupt.NVIC_IRQChannel = TIM4_IRQn;
    Interrupt.NVIC_IRQChannelCmd = ENABLE;
    Interrupt.NVIC_IRQChannelPreemptionPriority = 5;
    Interrupt.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&Interrupt);
    TIM_ClearITPendingBit(TIM4,TIM_IT_Update);

    TIM_Cmd(TIM4, ENABLE);
}

//FarbsortierungsInits
void FS_SORT_GpioInit(){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitTypeDef GPIO_UART;
    GPIO_UART.GPIO_Pin = GPIO_Pin_9;
    GPIO_UART.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_UART);

    GPIO_UART.GPIO_Pin = GPIO_Pin_10;
    GPIO_UART.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_UART);
}

void FS_SORT_TimerInit(){
    //CAN Clock init
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

    TIM_TimeBaseInitTypeDef TimInit;
    TIM_TimeBaseStructInit(&TimInit);
    TimInit.TIM_Period = 2500 - 1;
    TimInit.TIM_Prescaler = 7200 - 1;
    TIM_TimeBaseInit(TIM3,&TimInit);
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM3,DISABLE);
}

void FS_SORT_UartInit(){

    USART_InitTypeDef UART;
    UART.USART_BaudRate = 9600;
    UART.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    UART.USART_Parity = USART_Parity_No;
    UART.USART_StopBits = USART_StopBits_1;
    UART.USART_WordLength = USART_WordLength_8b;
    UART.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &UART);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    USART_Cmd(USART1, ENABLE);
}

void FS_SORT_IntInit(){
    /*
        Timer Interrupts
    */
    TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

    NVIC_InitTypeDef Interrupt;
    Interrupt.NVIC_IRQChannel = TIM3_IRQn;
    Interrupt.NVIC_IRQChannelCmd = ENABLE;
    Interrupt.NVIC_IRQChannelPreemptionPriority = 5;
    Interrupt.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&Interrupt);
    TIM_ClearITPendingBit(TIM3,TIM_IT_Update);


    NVIC_InitTypeDef UART;
    UART.NVIC_IRQChannel = USART1_IRQn;
    UART.NVIC_IRQChannelCmd = ENABLE;
    UART.NVIC_IRQChannelPreemptionPriority = 3;
    UART.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&UART);
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);
}

//KommissionierungsInits
void FS_KOM_GpioInit(){
    //Nur für Testzwecke
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);

    GPIO_InitTypeDef Buttons;
    Buttons.GPIO_Speed = GPIO_Speed_50MHz;
    Buttons.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    Buttons.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_Init(GPIOE, &Buttons);
}

void FS_KOM_TimerInit(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);

    TIM_TimeBaseInitTypeDef TimInit;
    TIM_TimeBaseStructInit(&TimInit);
    TimInit.TIM_Period = 10000 - 1;
    TimInit.TIM_Prescaler = 7200-1;

    TIM_SelectOnePulseMode(TIM2,TIM_OPMode_Single);

    TIM_TimeBaseInit(TIM2,&TimInit);
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);
    TIM_Cmd(TIM2,DISABLE);
}

void FS_KOM_IntInit(){
    /*
            Timer Interrupts
    */
    NVIC_InitTypeDef Interrupt;
    Interrupt.NVIC_IRQChannel = TIM2_IRQn;
    Interrupt.NVIC_IRQChannelCmd = ENABLE;
    Interrupt.NVIC_IRQChannelPreemptionPriority = 5;
    Interrupt.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&Interrupt);
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);

    //GPIO

    //Connect GPIOE-12 to EXTI (TEST)
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource10);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource11);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource14);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource15);

    //Connect Exti to GPIO / Connect Exti to NVIC (TEST)
    EXTI_InitTypeDef GPIOEInit12;
    EXTI_StructInit(&GPIOEInit12);
    GPIOEInit12.EXTI_Line = EXTI_Line10 | EXTI_Line11|EXTI_Line15|EXTI_Line14;
    GPIOEInit12.EXTI_LineCmd = ENABLE;
    EXTI_Init(&GPIOEInit12);

    //Connect NVIC to Exti (TEST)
    NVIC_InitTypeDef NVIC_EXTI_GPIOE12;
    NVIC_EXTI_GPIOE12.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_EXTI_GPIOE12.NVIC_IRQChannelCmd = ENABLE;
    NVIC_EXTI_GPIOE12.NVIC_IRQChannelPreemptionPriority = 10;
    NVIC_EXTI_GPIOE12.NVIC_IRQChannelSubPriority = 0;
    NVIC_Init(&NVIC_EXTI_GPIOE12);
}

//EntleerungsInits -- to add
void FS_ENTLEER_Init(){
//    FS_ENTLEER_WERKSTUECKVERLUST = false;
    FS_ENTLEER_ExtraTickRequest = true;

}

void FS_ENTLEER_TimerInit(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    TIM_Cmd(TIM6, DISABLE);

    TIM_TimeBaseInitTypeDef FS_ENTLEER_TimInit;
    TIM_TimeBaseStructInit(&FS_ENTLEER_TimInit);
    FS_ENTLEER_TimInit.TIM_Period = 60000 - 1;
    FS_ENTLEER_TimInit.TIM_Prescaler = 6000 - 1;
    TIM_TimeBaseInit(TIM6, &FS_ENTLEER_TimInit);
}

void FS_ENTLEER_IntInit(){
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    NVIC_InitTypeDef FS_ENTLEER_NVICInit;
    FS_ENTLEER_NVICInit.NVIC_IRQChannel = TIM6_IRQn;
    FS_ENTLEER_NVICInit.NVIC_IRQChannelPreemptionPriority = 5;
    FS_ENTLEER_NVICInit.NVIC_IRQChannelSubPriority = 0;
    FS_ENTLEER_NVICInit.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&FS_ENTLEER_NVICInit);
    TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
}

void FS_GpioInit(){
    FS_KOM_GpioInit();
    FS_SORT_GpioInit();
    FS_SERVO_GpioInit();
}

void FS_TimerInit(){
    FS_KOM_TimerInit();
    FS_SORT_TimerInit();
    FS_SERVO_TimerInit();
    FS_ENTLEER_TimerInit();
}

void FS_UartInit(){
    FS_SORT_UartInit();
}

void FS_IntInit(){
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    RCC_APB2PeriphClockCmd(RCC_APB2ENR_AFIOEN,ENABLE);

    FS_KOM_IntInit();
    FS_SORT_IntInit();
    FS_SERVO_IntInit();
    FS_ENTLEER_IntInit();
}

void FS_ProjectInit(){
    FS_GpioInit();
    FS_TimerInit();
    FS_UartInit();
    FS_IntInit();
    CAN_Init4Models();
    FS_ENTLEER_Init();

    FS_SORT_isOrder = false;
}


