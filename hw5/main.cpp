#include "mbed.h"
#include "TCPSocket.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include "MQTTClient.h"

WiFiInterface *wifi;
char *intTostring(int num);
void displayScreen();

int arrivedcount = 0;
char temp[30];
int arr[30] = { 0 }; 
// callback for subscribe topic
void subscribeCallback(MQTT::MessageData& md)
{

    MQTT::Message &message = md.message;
    printf("[sub] Payload %.*s\n", message.payloadlen, (char*)message.payload);
    sprintf(temp,"%.*s",message.payloadlen, (char*)message.payload);

    int j = 0, i, sum = 0; 
    // Traverse the string 
    for (i = 0; temp[i] != '\0'; i++) { 
        // if str[i] is ', ' then split 
        if (temp[i] == ',') 
            continue; 
         if (temp[i] == ' '){ 
            // Increment j to point to next 
            // array location 
            j++; 
        } 
        else { 
            arr[j] = arr[j] * 10 + (temp[i] - 48); 
        } 
    }
    for(int k =0; k < 4;k++){
        printf("%d\n",arr[k]);
    } 
    displayScreen();
}

int main()
{
    int count = 0;
    char* topic = "MQTT/sub";

    printf("WiFi MQTT example\n");

    #ifdef MBED_MAJOR_VERSION
        printf("Mbed OS version %d.%d.%d\n\n", MBED_MAJOR_VERSION, MBED_MINOR_VERSION, MBED_PATCH_VERSION);
    #endif

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) 
    {
        printf("ERROR: No WiFiInterface found.\n");
        return -1;
    }

    printf("\nConnecting to %s...\n", MBED_CONF_APP_WIFI_SSID);
    int ret = wifi->connect(MBED_CONF_APP_WIFI_SSID, MBED_CONF_APP_WIFI_PASSWORD, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) 
    {
        printf("\nConnection error: %d\n", ret);
        return -1;
    }

    printf("Success\n\n");
    printf("MAC: %s\n", wifi->get_mac_address());
    printf("IP: %s\n", wifi->get_ip_address());
    printf("Netmask: %s\n", wifi->get_netmask());
    printf("Gateway: %s\n", wifi->get_gateway());
    printf("RSSI: %d\n\n", wifi->get_rssi());

    MQTTNetwork mqttNetwork(wifi);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "192.168.88.8";
    int port = 1883;
    printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
    {
        printf("rc from TCP connect is %d\r\n", rc);
    }
        
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-sample";
    data.username.cstring = "";
    data.password.cstring = "";
    
    if ((rc = client.connect(data)) != 0)
    {
        printf("rc from MQTT connect is %d\r\n", rc);
    }
    else
    {
        printf("Client Connected.\r\n");
    }
    
    BSP_LCD_Init();    
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    } 

    if ((rc = client.subscribe(topic, MQTT::QOS0, subscribeCallback)) != 0)
    {
        printf("rc from MQTT subscribe is %d\r\n", rc);
    }
    else
    {
        printf("Client subscribed.\r\n");
    }

        

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;

    client.yield(500000); // This will also allow messages to be received. and wait to receive in milliseconds.
    
    printf("Finishing with %d messages received\n", arrivedcount);
    mqttNetwork.disconnect();

    if ((rc = client.disconnect()) != 0)
    {
        printf("rc from disconnect was %d\r\n", rc);
        printf("Client Disconnected.\r\n");
    }

    wifi->disconnect();

    printf("\nDone\n");
}

char *intTostring(int num){
    char te[20]; 
    sprintf(te, "%d", num);
    char *result = (char *) malloc(sizeof(char) * 15);
    strcpy(result,te);

    return result;
}

void displayScreen(){
    char *confirmed = intTostring(arr[0]);
    char *recovered = intTostring(arr[1]);
    char *death = intTostring(arr[2]);
    char *hospitalized = intTostring(arr[3]);
    

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 20, (uint8_t *)"Total Confirmed", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,40, (uint8_t *)confirmed, CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 80, (uint8_t *)"Recovered", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,100, (uint8_t *)recovered, CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 140, (uint8_t *)"Death", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,160, (uint8_t *)death, CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,200, (uint8_t *)"Hospitalized", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,220, (uint8_t *)hospitalized, CENTER_MODE);
}