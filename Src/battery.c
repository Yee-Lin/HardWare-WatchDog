#include "main.h"
#include "battery.h"

static const float HIGHEST_VOLTAGE = 58.8f;
static const float LOWEST_VOLTAGE = 46.0f;

float GetRemainCapacity(void)
{
	float capacity;
//	float voltage = GetVoltage();
//	if(voltage > HIGHEST_VOLTAGE) capacity = 1.0f;
//	else if(voltage <= LOWEST_VOLTAGE) voltage = 0.05f;
//	else
//	{
//		capacity = (voltage - LOWEST_VOLTAGE) / (HIGHEST_VOLTAGE - LOWEST_VOLTAGE);
//	}
	return capacity;
}
