#include "HX711.h"

#define GapValue  210.8 
extern uint32_t init_weight;
uint32_t weight;
uint32_t HX711_ReadData(void)
{
  uint32_t Count;
  uint8_t i;
  HX711_W_SCK(0);
  Count = 0;
  while(HX711_R_DO());
  for (i = 0; i < 24; i++)
  {
    HX711_W_SCK(1);
    Count = Count << 1;
    HX711_W_SCK(0);
    if (HX711_R_DO()) Count++;
  }
  
  HX711_W_SCK(1);
  Count = Count ^ 0x800000;
  HX711_W_SCK(0);
  return Count;
}

void HX711_Init_Weight(void)
{
  init_weight = HX711_ReadData();
}

void HX711_GetWeight(void)
{
  int32_t weight_temp;
  // 读取传感器原始值
  weight_temp = (int32_t)HX711_ReadData() - (int32_t)init_weight;
  

  if(weight_temp < 50 && weight_temp > -50) 
  {
    weight = 0;
  }
  else
  {

    weight = (uint32_t)( (float)abs(weight_temp) / GapValue );
  }
}