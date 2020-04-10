#include "mbed.h"
#include "TCPSocket.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "stm32f413h_discovery.h"
#include "stm32f413h_discovery_ts.h"
#include "stm32f413h_discovery_lcd.h"
#include "DHT11.h"
#include "MQTTClient.h"
DigitalOut led1(LED1);
char *intTostring(int num);
void displayScreen();
void closeScreen();

WiFiInterface *wifi;
DigitalIn  User_Button(PA_0,PullNone);
int arrivedcount = 0;
int aircon_status = 0;
float temp = 0,humid =0;
Dht11 sensor(D9);

Thread subscribe;

void subscribeCallback(MQTT::MessageData& md)
{
    char temp[50];
    MQTT::Message &message = md.message;
    printf("[sub] Payload %.*s\n", message.payloadlen, (char*)message.payload);
    sprintf(temp,"%.*s",message.payloadlen, (char*)message.payload);
    aircon_status = temp[0] - '0';
    ++arrivedcount;
}

void subscribe_thrad(){
    MQTTNetwork mqttNetwork(wifi);
    char* topic = "MQTT/sub";
    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "192.168.88.8";
    int port = 1883;
    printf("[sub] Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
    {
        printf("[sub] rc from TCP connect is %d\r\n", rc);
    }
        
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-sub";
    data.username.cstring = "";
    data.password.cstring = "";
    
    if ((rc = client.connect(data)) != 0)
    {
        printf("[sub] rc from MQTT connect is %d\r\n", rc);
    }
    else
    {
        printf("[sub] Client Connected.\r\n");
    }
        

    if ((rc = client.subscribe(topic, MQTT::QOS0, subscribeCallback)) != 0)
    {
        printf("[sub] rc from MQTT subscribe is %d\r\n", rc);
    }
    else
    {
        printf("[sub] Client subscribed.\r\n");
    }
        

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
    while(1)
        client.yield(500000);

    
}
const char *sec2str(nsapi_security_t sec)
{
    switch (sec) {
        case NSAPI_SECURITY_NONE:
            return "None";
        case NSAPI_SECURITY_WEP:
            return "WEP";
        case NSAPI_SECURITY_WPA:
            return "WPA";
        case NSAPI_SECURITY_WPA2:
            return "WPA2";
        case NSAPI_SECURITY_WPA_WPA2:
            return "WPA/WPA2";
        case NSAPI_SECURITY_UNKNOWN:
        default:
            return "Unknown";
    }
}

int scan_demo(WiFiInterface *wifi)
{
    WiFiAccessPoint *ap;
    printf("Scan:\n");
    int count = wifi->scan(NULL,0);
    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }
    count = count < 15 ? count : 15;
    ap = new WiFiAccessPoint[count];
    count = wifi->scan(ap, count);

    if (count <= 0) {
        printf("scan() failed with return value: %d\n", count);
        return 0;
    }

    for (int i = 0; i < count; i++) {
        printf("Network: %s secured: %s BSSID: %hhX:%hhX:%hhX:%hhx:%hhx:%hhx RSSI: %hhd Ch: %hhd\n", ap[i].get_ssid(),
               sec2str(ap[i].get_security()), ap[i].get_bssid()[0], ap[i].get_bssid()[1], ap[i].get_bssid()[2],
               ap[i].get_bssid()[3], ap[i].get_bssid()[4], ap[i].get_bssid()[5], ap[i].get_rssi(), ap[i].get_channel());
    }
    printf("%d networks available.\n", count);

    delete[] ap;
    return count;
}

int main()
{
   int count = 0;
    char* topic = "MQTT/pub";

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

    BSP_LCD_Init();    
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_ERROR) {
        printf("BSP_TS_Init error\n");
    } 

    MQTTNetwork mqttNetwork(wifi);

    MQTT::Client<MQTTNetwork, Countdown> client(mqttNetwork);

    const char* hostname = "192.168.88.8";
    int port = 1883;
    printf("Connecting to %s:%d\r\n", hostname, port);
    int rc = mqttNetwork.connect(hostname, port);
    if (rc != 0)
    {
        printf("[pub] rc from TCP connect is %d\r\n", rc);
    }
        
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "mbed-pub";
    data.username.cstring = "";
    data.password.cstring = "";
    
    if ((rc = client.connect(data)) != 0)
    {
        printf("[pub] rc from MQTT connect is %d\r\n", rc);
    }
    else
    {
        printf("[pub] Client Connected.\r\n");
    }
    
    subscribe.start(subscribe_thrad);

    MQTT::Message message;
    message.qos = MQTT::QOS0;
    message.retained = false;
    message.dup = false;
     while(true) // client.isConnected()
    {
        if(User_Button.read()== 1){
            led1.write(1);
            closeScreen();
            break;
        }
        if(aircon_status == 1){
            led1.write(1);
        }
        if(aircon_status == 0){
            led1.write(0);
        }

        sensor.read();
        temp = sensor.getCelsius();
        humid = sensor.getHumidity();
        displayScreen();
        char buf[100];
        sprintf(buf, "{\"Temp\": %0.2f, \"Humi\": %0.2f, \"Status\": %d}", temp, humid, aircon_status);
        printf("[pub]Clien sent->%s\n",buf);
        message.payload = buf;
        message.payloadlen = strlen(buf);
        rc = client.publish(topic, message);
        count++;
        //Wait for 2 seconds.
        wait(5);
    }
 
    
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
    char temp[10]; 
    sprintf(temp, "%d", num);
    char *result = (char *) malloc(sizeof(char) * 15);
    strcpy(result,temp);

    return result;
}

void displayScreen(){
    char *temper = intTostring(temp);
    char *humidity = intTostring(humid);
    char status[5] = "";

    if(aircon_status == 0){
        strcpy(status,"OFF");
    }
    else if(aircon_status == 1){
        strcpy(status,"ON");
    }

    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_BLACK);
    BSP_LCD_SetFont(&Font16);
    BSP_LCD_DisplayStringAt(0, 20, (uint8_t *)"TEMPERATURE", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,60, (uint8_t *)temper, CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 100, (uint8_t *)"HUMIDITY", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,140, (uint8_t *)humidity, CENTER_MODE);
    BSP_LCD_DisplayStringAt(0, 180, (uint8_t *)"A/C STATUS", CENTER_MODE);
    BSP_LCD_DisplayStringAt(0,220, (uint8_t *)status, CENTER_MODE);
}

void closeScreen(){
    BSP_LCD_Clear(LCD_COLOR_BLACK);
}