#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
 
TS_StateTypeDef  TS_State = {0};
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalIn  User_Button(PA_0,PullNone);
Timer t;

int main() {
    int start = 0;
    int timer_start =0;
    int count = 0;
    BSP_LCD_Init();    
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }
    BSP_LCD_Clear(LCD_COLOR_WHITE);

  while(true) {  
    if( User_Button.read() == 1 && start == 0){
        t.start();
        timer_start = 1;
        start = 1;
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
        BSP_LCD_FillRect(0, 0, BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
        BSP_LCD_SetFont(&Font16);
        BSP_LCD_DisplayStringAt(0, 15, (uint8_t *)"Touch the screen", CENTER_MODE);

    }

    if(timer_start == 1 && t.read() <= 5 ){
        if(TS_State.touchDetected){
            count++;
        }
    }

  }
}