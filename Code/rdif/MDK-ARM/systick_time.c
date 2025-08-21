#include "stm32f10x.h"           
#include "systick_time.h"

void systick_init(void)
{
	SysTick->CTRL=0;
	SysTick->LOAD=0xFFFFFF;
	SysTick->VAL=0;
	SysTick->CTRL=5;
}
void delaymillis(void)
{
	SysTick->LOAD=72000;
	SysTick->VAL=0;
	while(!(SysTick->CTRL&(1<<16)));
}
void delay(int t)
{
	for (;t>0;t--)
	{
		delaymillis();
	}
}
void delaymicro(void)
{
	SysTick->LOAD=72;
	SysTick->VAL=0;
	while(!(SysTick->CTRL&(1<<16)));
}
void delay_us(int y)
{
	for (;y>0;y--)
	{
		delaymicro();
	}
}