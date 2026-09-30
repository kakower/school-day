#include "servo.h"
extern TIM_HandleTypeDef htim4;
uint8_t switch_flag;
void Servo_SetAnagl(float Angal)
{
	__HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_1, Angal / 180 * 2000 + 500);
}


void Servo_Start(float Angal)
{
  HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_1);
  Servo_SetAnagl(Angal);
}

void Servo_Stop(void)
{
  HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_1);
}
uint8_t servo_time;
void Servo_Switch(void)
{
  if(switch_flag == 1)
  {
    Servo_Start(90);
    if (servo_time < 20)
    {
      servo_time++;
      return ;
    }
    servo_time = 0;
    switch_flag  = 0;
    Servo_Start(0);
  }
}

