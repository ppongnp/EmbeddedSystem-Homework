#include "mbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"

void display_screen(int mode);
void updateValue(const char* name);
void update_thread(void const *args);
void screen_setup(uint16_t textColor,uint16_t BackgroundColor);
char *intTostring(int num,int mode);


Mutex stdio_mutex;
Thread temp;
Thread humid;
Thread light;
TS_StateTypeDef  TS_State = {0};

int temp_value,humid_value,light_value;

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
    stdio_mutex.lock();
    if ( strcmp(name,"Temperature") == 0) {
        temp_value = rand() % 50;
    }
    else if( strcmp(name, "Humidity") == 0) {
        humid_value = rand() % 100; 
    }
    else if ( strcmp(name,"LightIntensity") == 0) {
        light_value = rand() % 100000;
    }
    else{
        temp_value = 0;
        humid_value = 0;
        light_value = 0;
    }
    stdio_mutex.unlock();
}

void update_thread(void const *args)
 {
    while (true) 
    {
        updateValue((const char*)args); 
        thread_sleep_for(100);
    }
}

void display_screen(int mode){
    if (mode == 1){
        char *returned_result = intTostring(temp_value, mode);
        screen_setup(LCD_COLOR_WHITE, LCD_COLOR_RED);
        BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"TEMPERATURE", CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,200, (uint8_t *)returned_result, CENTER_MODE);
    }
    else if (mode == 2){
        char *returned_result = intTostring(temp_value, mode);
        screen_setup(LCD_COLOR_WHITE, LCD_COLOR_BLUE);
        BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"HUMIDITY", CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,200, (uint8_t *)returned_result, CENTER_MODE);
    }
    else if (mode == 3){
        char *returned_result = intTostring(temp_value, mode);
        screen_setup(LCD_COLOR_YELLOW, LCD_COLOR_BLACK);
        BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"LIGHT INTENSITY", CENTER_MODE);
        BSP_LCD_DisplayStringAt(0,200, (uint8_t *)returned_result, CENTER_MODE);
    }
}

void screen_setup(uint16_t textColor,uint16_t BackgroundColor){
    BSP_LCD_Clear(BackgroundColor);
    BSP_LCD_SetTextColor(textColor);
    BSP_LCD_SetBackColor(BackgroundColor);
    BSP_LCD_SetFont(&Font16);
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
