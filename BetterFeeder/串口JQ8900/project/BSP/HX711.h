#ifndef __HX711_H
#define __HX711_H
#include "stm32f1xx_hal.h"


#define HX711_GPIOx           GPIOB
#define HX711_SCK_GPIO_Pin     GPIO_PIN_5
#define HX711_DO_GPIO_Pin      GPIO_PIN_4


#define HX711_W_SCK(x)    HAL_GPIO_WritePin(HX711_GPIOx, HX711_SCK_GPIO_Pin, (GPIO_PinState)x)
#define HX711_R_DO(x)     HAL_GPIO_ReadPin(HX711_GPIOx, HX711_DO_GPIO_Pin)
uint32_t HX711_ReadData(void);
void HX711_Init_Weight(void);

void HX711_GetWeight(void);
#endif  /* __HX711_H */

