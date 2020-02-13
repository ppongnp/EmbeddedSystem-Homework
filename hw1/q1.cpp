#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

TS_StateTypeDef  TS_State = {0};
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);

int main()
{
    
    uint16_t x1, y1,x_pos,y_pos;
    BSP_LCD_Init();

    /* Touchscreen initialization */
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    }
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawVLine(80,0,240);
    BSP_LCD_DrawVLine(160,0,240);

    while (1) {
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected) {
            x_pos = TS_State.touchX[0];
            if(x_pos <= 80){
                led1.write(1);
                led2.write(0);
                led3.write(0);
            }
            else if(x_pos > 80 && x_pos <= 160){
                led1.write(0);
                led2.write(1);
                led3.write(0);
            }
            else if(x_pos > 160){
                led1.write(0);
                led2.write(0);
                led3.write(1);
            }
            thread_sleep_for(10);
        }
        else{
            led1.write(0);
            led2.write(0);
            led3.write(0);
        }
    }
}
