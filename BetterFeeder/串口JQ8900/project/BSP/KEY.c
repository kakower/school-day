#include "KEY.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "OLED.h"
#include "SERVO.h"
struct keys key[KEY_MAX_NUMBER];
extern uint8_t  switch_flag;

void Key_Scan(void)
{
  if (key[0].key_flag == 1)
  {
    switch_flag = 1;
    key[0].key_flag = 0;
  }
  
  else if (key[1].key_flag == 1)
  {
    key[1].key_flag = 0;
  }
 
  else
  {
    key[0].key_flag = 0;
    key[1].key_flag = 0;
  }
  
}




