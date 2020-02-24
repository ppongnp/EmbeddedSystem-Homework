#include "mbed.h"

#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

void display_screen(int mode);
void updateValue(const char* name);
void update_thread(void const *args);
void screen_setup(uint16_t textColor,uint16_t BackgroundColor);
void bar_setup();
char *intTostring(int num,int mode);

Semaphore one_slot(2);
Thread temp;
Thread humid;
Thread light;
TS_StateTypeDef  TS_State = {0};
uint16_t semaphore_color[] = {LCD_COLOR_BLUE,LCD_COLOR_RED};
int temp_value,humid_value,light_value;
int first = 0,second = 1;

void swapValue(int &a, int &b) {
   int t = a;
   a = b;
   b = t;
}

int main() 
{
    int count = 0;
    int mode[3] = {1,2,3};


    temp.start(callback(update_thread, (void *)"Temperature"));
    humid.start(callback(update_thread, (void *)"Humidity"));
    light.start(callback(update_thread, (void *)"LightIntensity"));

    BSP_LCD_Init();    
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    } 
    display_screen(mode[0]);
    
    while(true){
        BSP_TS_GetState(&TS_State);
        if(TS_State.touchDetected){
            count += 1;
            if(count > 3){
                count = 0;
            }
            display_screen(mode[count]);
        }
        thread_sleep_for(200);
    }
}

void updateValue(const char* name) 
{
    one_slot.wait();

    swapValue(first, second);
    BSP_LCD_SetTextColor(semaphore_color[first]);

    temp_value = rand() % 50;
    char temp1[10]; 
    sprintf(temp1, "%d", temp_value);

    humid_value = rand() % 100; 
    char temp2[10]; 
    sprintf(temp2, "%d", humid_value);

    light_value = rand() % (10000 + 1 - 1000) + 1000;
    char temp3[10]; 
    sprintf(temp3, "%d", light_value);

    BSP_LCD_DisplayStringAt(10, 220, (uint8_t *)temp1,LEFT_MODE);
    BSP_LCD_DisplayStringAt((BSP_LCD_GetXSize()/3) + 10, 220, (uint8_t *)temp2,LEFT_MODE);
    BSP_LCD_DisplayStringAt(((BSP_LCD_GetXSize() * 2) /3) + 10, 220, (uint8_t *)temp3,LEFT_MODE);
    
    thread_sleep_for(1000);
    one_slot.release();
    
}

void update_thread(void const *args)
 {
    while (true) 
    {
        updateValue((const char*)args); 
        thread_sleep_for(1000);
    }
}

void display_screen(int mode){
    if (mode == 1){
        char *returned_result = intTostring(temp_value, mode);
        screen_setup(LCD_COLOR_WHITE, LCD_COLOR_RED);
        BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)"TEMPERATURE", CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,160, (uint8_t *)returned_result, CENTER_MODE);
        bar_setup();
    }
    else if (mode == 2){
        char *returned_result = intTostring(temp_value, mode);
        screen_setup(LCD_COLOR_WHITE, LCD_COLOR_BLUE);
        BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)"HUMIDITY", CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,160, (uint8_t *)returned_result, CENTER_MODE);
        bar_setup();
    }
    else if (mode == 3){
        char *returned_result = intTostring(temp_value, mode);
        screen_setup(LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(0, 40, (uint8_t *)"LIGHT INTENSITY", CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,160, (uint8_t *)returned_result, CENTER_MODE);
        bar_setup();
    }
}

void screen_setup(uint16_t textColor,uint16_t BackgroundColor){
    BSP_LCD_Clear(BackgroundColor);
    BSP_LCD_SetTextColor(textColor);
    BSP_LCD_SetBackColor(BackgroundColor);
    BSP_LCD_SetFont(&Font16);
}

void bar_setup(){
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_FillRect(0, BSP_LCD_GetYSize() - 30, BSP_LCD_GetXSize(), 30);
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DrawVLine(BSP_LCD_GetXSize()/3,BSP_LCD_GetYSize() - 30,30);
    BSP_LCD_DrawVLine((BSP_LCD_GetXSize() * 2) /3,BSP_LCD_GetYSize() - 30,30);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
}

char *intTostring(int num,int mode){
    char temp[10]; 
    sprintf(temp, "%d", num);
    if (mode == 1) {
        char unit[] = " C";
        strcat(temp,unit);
    }
    else if (mode == 2) {
        char unit[] = " %/rh";
        strcat(temp,unit);
    }
    else if (mode == 3){
        char unit[] = " lx";
        strcat(temp,unit);
    }
    char *result = (char *) malloc(sizeof(char) * 15);
    strcpy(result,temp);

    return result;
}
