/*#include "mbed.h"

Thread thread;
DigitalOut led(LED1);

void led_thread()
 {
    while (true)
     {
        // Signal flags that are reported as event are automatically cleared.
        Thread::signal_wait(0x1);
        led = !led;
    }
}

int main (void) 
{
    thread.start(callback(led_thread));

    while (true) 
    {
        thread_sleep_for(1000);
        thread.signal_set(0x1);
    }
}
*/