#include "main.h"
#include "stm32f4xx_hal.h"

const uint32_t middlePPM = 760;
uint32_t ic3Value1 = 0, ic3Value2 = 0, ppm0 = 0;
uint32_t ic4Value1 = 0, ic4Value2 = 0, ppm1 = 0;
extern TIM_HandleTypeDef htim3;

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_3)
	{
		//读取此时IO口的电平,为高,则这次的捕获是上升沿
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET)
		{
			/* Get the 1st Input Capture value */
			ic3Value1 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_3);
		}
		else
		{	//下降沿
			/* Get the 2st Input Capture value */
			ic3Value2 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_3);
			//2大于1,说明在同一个计数周期内,直接减
			if(ic3Value2 > ic3Value1)
				ppm0 = ic3Value2 - ic3Value1; 
			else
			{ //2小于1,不在一个计数周期内,用周期减1的计数,再减去2,得出真实脉宽
				ppm0 = (__HAL_TIM_GET_AUTORELOAD(&htim3) -ic3Value1 + 1) + ic3Value2; 
			}
		}
	}
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_4)
	{
		//读取此时IO口的电平,为高,则这次的捕获是上升沿
		if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_SET)
		{
			/* Get the 1st Input Capture value */
			ic4Value1 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_4);
		}
		else
		{	//下降沿
			/* Get the 2st Input Capture value */
			ic4Value2 = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_4);
			//2大于1,说明在同一个计数周期内,直接减
			if(ic4Value2 > ic4Value1)
				ppm1 = ic4Value2 - ic4Value1; 
			else
			{ //2小于1,不在一个计数周期内,用周期减1的计数,再减去2,得出真实脉宽
				ppm1 = (__HAL_TIM_GET_AUTORELOAD(&htim3) -ic4Value1 + 1) + ic4Value2; 
			}
		}
	}
}
