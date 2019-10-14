#ifndef ESP_8266_h
#define ES_8266P_h

#define TIMEOUT 10  // attempts to execute AT command
#define SENDING_INTERVAL 30000  // how often new data will be sent to the server

#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ReadingsQueue.h>
#include <Utils.h>
#include <Log.h>

class ESP
{
    public:
        ESP(int rx, int tx);
        
        char connection_status();

        bool restart();
        bool connect_wifi(const char *ssid, const char *pwd);
        bool connect_host(const char *address);

        bool execute_at(const char *cmd);
        bool execute_at(const char *cmd, const char *escape);

        bool send_post_request(const char *host, const ReadingsQueue &queue);
        bool connected();

    private:
        SoftwareSerial *_self;
        int _status;

        char _command[512];
        char _post_data[256];
        char _last_status[256];
        
        bool _is_connected; 
        bool _execute_at(const char *cmd, const char *escape);

        void _handle_err(const String *msg);

};

#endif