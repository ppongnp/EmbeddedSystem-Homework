#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

TS_StateTypeDef  TS_State = {0};
DigitalIn  User_Button(PA_0,PullNone);
Timeout t;
int score;
bool start;

void reset();
void start_and_end_screen_setup();

int main() {
    start = false;
    score = 0;
    BSP_LCD_Init();    
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }
    start_and_end_screen_setup();
    BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"PRESS BUTTON TO START", CENTER_MODE);

    while(true) {  
      BSP_TS_GetState(&TS_State);
      if(User_Button.read() == 1 && start == false){ 
          start = true;
          t.attach(&reset, 5.0);
          BSP_LCD_Clear(LCD_COLOR_RED);
          BSP_LCD_SetFont(&Font16);
          BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
          BSP_LCD_SetBackColor(LCD_COLOR_RED);  
          BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)"TOUCH HERE !!", CENTER_MODE);
      }
      if(TS_State.touchDetected && start == true){
          score += 1;
          char temp[20]; 
          sprintf(temp, "%d", score);
          BSP_LCD_Clear(LCD_COLOR_RED);
          BSP_LCD_SetFont(&Font24);
          BSP_LCD_DisplayStringAt(0, 120, (uint8_t *)temp, CENTER_MODE);
          thread_sleep_for(100);
      }
  }
}
void start_and_end_screen_setup(){
    BSP_LCD_Clear(LCD_COLOR_BLUE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLUE);    
}

void reset(){
    t.detach();
    start = false;

    char temp[20]; 
    sprintf(temp, "%d", score);
    char result[] = "YOUR SCORE ARE :";
    strcat(result, temp);

    start_and_end_screen_setup();
    BSP_LCD_SetFont(&Font24);
    BSP_LCD_DisplayStringAt(0, 60, (uint8_t *)"GAME OVER!", CENTER_MODE);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 160, (uint8_t *)result, CENTER_MODE);
    BSP_LCD_SetFont(&Font12);
    BSP_LCD_DisplayStringAt(0, 200, (uint8_t *)"PRESS BUTTON TO TRY AGAIN!", CENTER_MODE);

    score = 0;
}