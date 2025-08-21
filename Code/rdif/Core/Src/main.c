#include "main.h"
#include "rc522.h"
#include "string.h"
#include "keypad.h"
#include "lcd-i2c.h"
#include <stdio.h>
#include "systick_time.h"
#include <stdlib.h>
#include "servo.h"

SPI_HandleTypeDef hspi1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);

uint8_t status;
uint8_t str[MAX_LEN]; // Max_LEN = 16
uint8_t sNum[5];
uint8_t UID[5];		// Luu UID (4 byte + 1 byte checksum)
char chuoi[30];
char number[2];
volatile char num_flag=0;
int stage = 1;

char password[7] = {0}; // Luu m?t kh?u nh?p vào
char validpassword[7] = "888999";
uint8_t pass_index = 0; // V? trí ký t? m?t kh?u
char display[7] = "------"; // Hi?n th? trên LCD

int angle=0;
int count=0;

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
	MFRC522_Init();
	systick_init();
 	keypad_init();
	lcd_i2c_init();
	servo_start();
	
	RCC->APB2ENR|=RCC_APB2ENR_IOPAEN;
	GPIOA->CRL&=~(0xF<<0);
	GPIOA->CRL|=(1<<0);
	GPIOA->ODR&=~(1<<0);	// Turn off
	//GPIOA->ODR|=(1<<0);	// Turn on
	
	RCC->APB1ENR|=RCC_APB1ENR_TIM2EN;
	TIM2->PSC=1;
	TIM2->ARR=35999;
	TIM2->DIER|=(1<<0);
	NVIC->ISER[0]|=(1<<28);
	TIM2->CR1&=~(1<<0);
	

	
	
//z	/*Intro*/
//	xetgoc(160);
//	lcd_i2c_clear();
//	lcd_i2c_msg(1,3,"Smart Lock");
//	lcd_i2c_msg(2,2,"KS-IOT & VTN");
	
	
	
	
	
	/*Start*/
	xetgoc(156);
	lcd_i2c_clear();
	lcd_i2c_msg(1,3,"Smart Lock");
	lcd_i2c_msg(2,5,"SYSTEM");
	delay(1200);
	lcd_i2c_clear();
  while (1)
  {
		/* Kiem tra bang the RFID */
		while(stage==1)
		{
			lcd_i2c_msg(1,1,"Scan RFID card ");
			status = MFRC522_Request(PICC_REQIDL, str);
			if (status == MI_OK)
			{
				lcd_i2c_clear();
				lcd_i2c_msg(1,0,"Scanning");
				status = MFRC522_Anticoll(str);
				memcpy(sNum, str, 5);
				for (int i=0;i<5;i++)
				{
					lcd_i2c_msg(1,8+i,".");
					delay(300);
				}
				lcd_i2c_clear();
				if((str[0]==242) && (str[1]==182) && (str[2]==65) && (str[3]==2) && (str[4]==7) )
				{
					TIM2->CR1|=(1<<0);
					lcd_i2c_blink(1,2,"Card's VALID!",3,300);
					delay(300);
					stage =2;
					break;
				}
				else 
				{    
					for (uint8_t i = 0; i < 3; i++) 
					{
						GPIOA->ODR|=(1<<0);	
						delay(150);
						GPIOA->ODR&=~(1<<0);	
						delay(50);			
					}
					lcd_i2c_blink(1,1,"Card's INVALID!!",2,250);
					delay(500);
					lcd_i2c_msg(1,3,"Scan again!");
					delay(500);
					lcd_i2c_clear();
				}
			}
		}
		/* Kiem tra bang ban phim ma tran */
		while(stage==2)
		{
			pass_index=0;
			for (int i=0;i<6;i++)
			{
				display[i]='-';
			}
			num_flag=0;
			password[0]='\0';
			lcd_i2c_msg(1,1,"Enter password");
			while(pass_index<6)
			{
				lcd_i2c_msg(2,5,display);
				quet_keypad();
				if (num_flag)
				{
					num_flag=0;
					password[pass_index] = number[0];
					display[pass_index] = number[0];
					lcd_i2c_msg(2,5,display);
					delay(250);
					display[pass_index] = '*';
					lcd_i2c_msg(2,5,display);
					pass_index++;
				}
			}
			// Ki?m tra m?t kh?u
			lcd_i2c_clear();
			if (strcmp(password, validpassword) == 0) 
			{
				TIM2->CR1|=(1<<0);
				lcd_i2c_blink(1,1,"Right Password",2,300);
				delay(600);
				stage=3;
				break;
			}
			else 
			{
				for (uint8_t i = 0; i < 3; i++) 
				{
					GPIOA->ODR|=(1<<0);	
					delay(100);
					GPIOA->ODR&=~(1<<0);	
					delay(50);			
				}
				lcd_i2c_blink(1,1,"Wrong Password!",2,300);
				delay(500);
				lcd_i2c_clear();
				lcd_i2c_msg(1,3,"Try again !");
				delay(500);
				lcd_i2c_clear();
			}
		}
		
		/* Co cau chap hanh */
		while(stage==3)
		{
			lcd_i2c_msg(1,2,"Door Unlocked");
			xetgoc(0);
			delay(2000);
			lcd_i2c_clear();
			lcd_i2c_msg(1,0,"Door'll close in");
			lcd_i2c_msg(2,7,"4s");
			delay(1000);
			lcd_i2c_msg(2,7,"3s");
			delay(1000);
			lcd_i2c_msg(2,7,"2s");
			delay(1000);
			lcd_i2c_msg(2,7,"1s");
			delay(1000);
			lcd_i2c_msg(2,7,"0s");
			delay(100);
			xetgoc(156);
			delay(500);
			lcd_i2c_clear();
			lcd_i2c_msg(1,3,"Door Locked");
			delay(1000);
			stage=1;
			lcd_i2c_clear();
			delay(1000);	
		}
		

		
  }
}
void TIM2_IRQHandler(void)
{
	TIM2->SR&=~(1<<0);
	if (count>=500)
	{
		count=0;
		GPIOA->ODR&=~(1<<0);	
		TIM2->CR1&=~(1<<0);
	}
	else
	{
		count++;
		GPIOA->ODR|=(1<<0);	
	}
}
void EXTI15_10_IRQHandler(void)
{
	num_flag=1;
	if ((EXTI->PR&(1<<12)) == (1<<12))
	{
		if (!(GPIOA->ODR&(1<<8))) number[0]='1';
		else if (!(GPIOA->ODR&(1<<9))) number[0]='4';
		else if (!(GPIOA->ODR&(1<<10))) number[0]='7';
		else if (!(GPIOA->ODR&(1<<11))) number[0]='*';
		
	}
	else if ((EXTI->PR&(1<<13)) == (1<<13))
	{
		if (!(GPIOA->ODR&(1<<8))) number[0]='2';
		else if (!(GPIOA->ODR&(1<<9))) number[0]='5';
		else if (!(GPIOA->ODR&(1<<10))) number[0]='8';
		else if (!(GPIOA->ODR&(1<<11))) number[0]='0';
	}
	else if ((EXTI->PR&(1<<14)) == (1<<14))
	{
		if (!(GPIOA->ODR&(1<<8))) number[0]='3';
		else if (!(GPIOA->ODR&(1<<9))) number[0]='6';
		else if (!(GPIOA->ODR&(1<<10))) number[0]='9';
		else if (!(GPIOA->ODR&(1<<11))) number[0]='#';
	}
	else if ((EXTI->PR&(1<<15)) == (1<<15))
	{
		if (!(GPIOA->ODR&(1<<8))) number[0]='A';
		else if (!(GPIOA->ODR&(1<<9))) number[0]='B';
		else if (!(GPIOA->ODR&(1<<10))) number[0]='C';
		else if (!(GPIOA->ODR&(1<<11))) number[0]='D';
	}
//	for(volatile int i=0;i<1000;i++)
//	{
//		for(volatile int j=0;j<1000;j++);
//	}
	
	EXTI->PR|=(1<<12)|(1<<13)|(1<<14)|(1<<15);
}
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
