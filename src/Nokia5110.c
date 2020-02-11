/*
 *  Nokia5110.c
 *
 *  Created on: Jan 30, 2020
 *  Author: ÖZKAN YILDIRIM
 */

#include "Nokia5110.h"

extern SPI_HandleTypeDef hspi2;
extern DMA_HandleTypeDef hdma_spi2_tx;

bool Nokia5110_Init(void)
{
	Nokia5110_Reset();

	if(!Nokia5110_Write(0x21, 0))
		return false;
	if(!Nokia5110_Write(0xC0, 0))
		return false;
	if(!Nokia5110_Write(0x07, 0))
		return false;
	if(!Nokia5110_Write(0x13, 0))
		return false;
	if(!Nokia5110_Write(0x20, 0))
		return false;
	if(!Nokia5110_Write(0x0C, 0))
		return false;

	return true;

}
void Nokia5110_Reset(void)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_RESET);	//RESET pin is LOW
	HAL_Delay(100);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_8, GPIO_PIN_SET);		//RESET pin is HIGH

}
void Nokia5110_Clear(void)
{
	for(int i=0; i<504; i++)
	{
		frameBuff[i] = 0x00;
	}
}


void Nokia5110_SetPixel(uint8_t x , uint8_t y , bool set)
{
	uint8_t Bi;
	uint16_t By;

	if(x < 0 || x >=84 || y < 0 || y>= 84)
		return;

	By = (y/8) * 84 + x; 	// x = 32 y = 24
	Bi = y % 8;

	if(set)
	{
		frameBuff[By] |= (1 << Bi);
	}
	else
	{
		frameBuff[By]  &= ~(1 << Bi);
	}
}

bool Nokia5110_Update()
{
	// move to X = 0
	if(!Nokia5110_Write(0x08, 0))
		return false;
	//Move to Y = 0
	if(!Nokia5110_Write(0x04, 0))
		return false;

	Nokia5110_BufferWrite(frameBuff , 504);

	return true;
}

bool Nokia5110_Write(uint8_t data , uint8_t mode)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);	//CE pin is LOW

	if(mode == 0)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_RESET); 	//DC Pin is LOW data send
	else if(mode == 1)
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET); 	//DC Pin is HIGH command send
	else
	{
		HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);	//CE pin is HIGH
		return false;
	}

	HAL_SPI_Transmit_DMA(&hspi2, &data, 1);
}

void Nokia5110_BufferWrite(uint8_t* data , uint16_t lenght)
{
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_9, GPIO_PIN_RESET);	//CE pin is LOW
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_10, GPIO_PIN_SET); 	//DC Pin is HIGH command send
	HAL_SPI_Transmit_DMA(&hspi2, data, lenght);
}
