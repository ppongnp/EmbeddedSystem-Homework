/*#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
Thread thread,thread2, thread3;
uint16_t color[] ={LCD_COLOR_WHITE,LCD_COLOR_BLACK,LCD_COLOR_BLUE,LCD_COLOR_BROWN,LCD_COLOR_CYAN,LCD_COLOR_GRAY\
                    ,LCD_COLOR_GREEN,LCD_COLOR_MAGENTA,LCD_COLOR_ORANGE,LCD_COLOR_RED,LCD_COLOR_YELLOW};

void led2_thread() {
    while (true) 
    {
        led2 = !led2;
        thread_sleep_for(500);
    }
}
void led3_thread(){
    while (true) 
    {
        led3 = !led3;
        thread_sleep_for(750);
    }
}
void touchScreen_thread(){
    int pick;
    while(true){
        pick = rand()%sizeof(color);
        BSP_LCD_Clear(color[pick]);
        thread_sleep_for(500);
    }
}
 
int main()
{
    BSP_LCD_Init();    
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }
    thread.start(led2_thread);
    thread2.start(led3_thread);
    thread3.start(touchScreen_thread);
    
    while (true) 
    {
        led1 = !led1;
        thread_sleep_for(250);
    }
}*/