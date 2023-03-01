#include <stdint.h>
#include "stm32f4xx.h"

uint8_t Button_Flag = 0;

void delay(uint32_t time) {
	while (time--)
		;
}

/*========== Interrupt handlers =============*/

void EXTI0_IRQHandler(void)
{
	uint16_t count = 0;
	uint8_t PrevState = 0;

	while(count<30)
	{
		if (PrevState != (GPIOA->IDR & GPIO_IDR_ID0_Msk))
		{
			PrevState = (GPIOA->IDR & GPIO_IDR_ID0_Msk);
			count=0;
		}
		else
		{
			count++;
		}
		delay(500);
	}

	if (PrevState >= 1)
	{
		Button_Flag ^= 1;
	}

	EXTI->PR|=EXTI_PR_PR0;
	__NVIC_ClearPendingIRQ(EXTI0_IRQn);
}

/*=========== Startup settings ===========*/

void RCCConfig(void)
{
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOAEN;
}

void GPIOConfig(void) {

	GPIOD->MODER |= GPIO_MODER_MODE12_0 | GPIO_MODER_MODE13_0 | GPIO_MODER_MODE14_0 | GPIO_MODER_MODE15_0;

	GPIOD->OTYPER &= ~(GPIO_OTYPER_OT12 | GPIO_OTYPER_OT13 | GPIO_OTYPER_OT14 | GPIO_OTYPER_OT15);

	GPIOD->OSPEEDR |= GPIO_OSPEEDR_OSPEED12_0 | GPIO_OSPEEDR_OSPEED13_0	| GPIO_OSPEEDR_OSPEED14_0 | GPIO_OSPEEDR_OSPEED15_0;

	GPIOA->MODER&=~GPIO_MODER_MODE0;
}



void INTConfig(void) {
	RCC->APB2ENR|= RCC_APB2ENR_SYSCFGEN;

	SYSCFG->EXTICR[0]|=SYSCFG_EXTICR1_EXTI0_PA;

	EXTI->IMR|=EXTI_IMR_IM0;

	EXTI->RTSR|=EXTI_RTSR_TR0;

	__enable_irq();

	__NVIC_SetPriority(EXTI0_IRQn, 3);

	__NVIC_EnableIRQ(EXTI0_IRQn);
}

/*============ Main ==============*/

int main(void) {

	RCCConfig();
	GPIOConfig();
	INTConfig();

	uint8_t i = 12;

	while (1)
	{

		if (Button_Flag)
		{
			GPIOD->BSRR |= (1 << i++);
			if (i == 16) {
				i += 12;
			} else if (i == 32) {
				i = 12;
			}
			delay(1000000);
		}

	}

	return 0;
}
