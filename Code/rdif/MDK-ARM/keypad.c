#include "stm32f10x.h"
#include "keypad.h"
#include "systick_time.h"

void keypad_init(void)
{
	// Output: A8 A9 A10 A11
	RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
	GPIOA->CRH&=~(0xF<<0)&~(0xF<<4)&~(0xF<<8)&~(0xF<<12);
	GPIOA->CRH|=(1<<0)|(1<<4)|(1<<8)|(1<<12);
	GPIOA->ODR|=(1<<8)|(1<<9)|(1<<10)|(1<<11);
	
	// Input: B12 B13 B14 B15
	RCC->APB2ENR|=RCC_APB2ENR_IOPBEN;
	GPIOB->CRH&=~(0xF<<16)&~(0xF<<20)&~(0xF<<24)&~(0xF<<28);
	GPIOB->CRH|=(8<<16)|(8<<20)|(8<<24)|(8<<28);
	GPIOB->ODR|=(1<<12)|(1<<13)|(1<<14)|(1<<15);
	
	RCC->APB2ENR|=RCC_APB2ENR_AFIOEN;
	AFIO->EXTICR[3]|=(1<<0)|(1<<4)|(1<<8)|(1<<12);
	EXTI->IMR|=(1<<12)|(1<<13)|(1<<14)|(1<<15);
	EXTI->FTSR|=(1<<12)|(1<<13)|(1<<14)|(1<<15);
	NVIC->ISER[1]|=(1<<8);
}
void quet_keypad(void)
{
	GPIOA->ODR|=(1<<8)|(1<<9)|(1<<10);
	GPIOA->ODR&=~(1<<11);
	delay(1);
	delay_us(300);
	
	GPIOA->ODR|=(1<<8)|(1<<9)|(1<<11);
	GPIOA->ODR&=~(1<<10);
	delay(1);
	delay_us(300);
	
	GPIOA->ODR|=(1<<8)|(1<<11)|(1<<10);
	GPIOA->ODR&=~(1<<9);
	delay(1);
	delay_us(300);
	
	GPIOA->ODR|=(1<<11)|(1<<9)|(1<<10);
	GPIOA->ODR&=~(1<<8);
	delay(1);
	delay_us(300);
}

