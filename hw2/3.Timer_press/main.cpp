#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"


TS_StateTypeDef  TS_State = {0};
DigitalIn  User_Button(PA_0,PullNone);
Timeout t;
void reset();

int score;
int attr;
bool start;

int main() {
    start = false;
    score = 0;
    BSP_LCD_Init();    
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }
    BSP_LCD_Clear(LCD_COLOR_YELLOW);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"PRESS BUTTON TO START!", CENTER_MODE);

    while(true) {  
      BSP_TS_GetState(&TS_State);
      if(User_Button.read() == 1 && start == false){ 
          attr = 1;
          start = true;
          t.attach(&reset, 5.0);
          BSP_LCD_Clear(LCD_COLOR_WHITE);
          BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
          BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"gogogo", CENTER_MODE);
      }

      if(TS_State.touchDetected && start == true){
          score += 1;
          BSP_LCD_Clear(LCD_COLOR_WHITE);
          BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
          char s2[20]; 
          sprintf(s2, "%d", score); 
          BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)s2, CENTER_MODE);
          thread_sleep_for(100);
      }
  }
}

void reset(){
    t.detach();
    score = 0;
    start = false;
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"END", CENTER_MODE);
}