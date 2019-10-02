#ifndef ESP_8266_h
#define ES_8266P_h

#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ReadingsQueue.h>

class ESP
{
    public:
        ESP(int rx, int tx);
        
        int test();
        char connection_status();

        bool restart();
        bool connect_wifi(const String *ssid, const String *pwd);
        bool connect_host(const String address);

        bool execute_at(const String cmd);
        bool execute_at(const String cmd, const String escape, int i);

        bool send_post_request(const String host, ReadingsQueue *queue);
        bool connected();

    private:
        SoftwareSerial *_self;
        int _status;

        char _command[512];
        char _post_data[256];
        
        String _last_status;
        String _host;

        bool _connected;
        void _handle_err(const String *msg);
};

#endif