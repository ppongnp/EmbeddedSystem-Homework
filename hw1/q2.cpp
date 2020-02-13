#include "mbed.h"

#define PressButton 1
DigitalOut led1(LED1);
DigitalOut led2(LED2);
DigitalOut led3(LED3);
DigitalIn  User_Button(PA_0,PullNone);
// main() runs in its own thread in the OS
int main()
{
    int count = 0;
    int temp = 0;//if led is on
    while (true) {
        if(User_Button.read() == PressButton && temp == 0){
            count += 1;
            thread_sleep_for(1000);
            if(count == 3){
                led3.write(1);
            }   
        }
        else if(User_Button.read() == PressButton && temp == 1){
            led3.write(0);
            temp = 0;
        }
        else{
            count = 0;
            if(led3 == 1){
                temp = 1;
            }
        }
    }
}