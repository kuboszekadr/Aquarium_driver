#include "ESP_8266.h"

ESP::ESP(int rx_pin,  int tx_pin)
{
    _self = new SoftwareSerial (rx_pin, tx_pin);
    _self->begin(9600);
}

bool ESP::connected()
{
    return execute_at("AT");
}

bool ESP::restart()
{
    bool t = execute_at("AT+RST");
    bool x = execute_at("AT+CIPMODE=0");

    return (x && t);
}

bool ESP::execute_at(const String cmd)
{
    return execute_at(cmd, "OK", 0);
}

bool ESP::connect_wifi(const String *ssid, const String *pwd)
{
    _self->println("AT+CWJAP=\"" + *ssid + "\",\"" + *pwd + "\"");
    delay(100);

    String resp = "";
    int i = 0;

    while (true)
    {
        // check if new data is available
        if(_self->available() > 0)
        {   
            resp = _self->readString(); // read current return
            if(resp.indexOf("FAIL") > 0) // check if response is an error
            {
                _handle_err(&resp); // parse response to check error
                break;
            }
            else if (resp.indexOf("OK")>0) {return true; }
            
        }
        else {delay(500);} //wait for new data

        if (i == 50){Serial.println("Connection takes longer then expected...");}
        if (i >= 100)
        {
            Serial.println("Connection timeout");
            break;
        }
        i+=1;
    }
    return false;
}

bool ESP::connect_host(const String host)
{
    if ((int)connection_status() == int('3')) {return true; } // already connected
    return execute_at("AT+CIPSTART=\"TCP\",\"" + host + "\",80"); // open new connection
}

char ESP::connection_status()
{
    (void) execute_at("AT+CIPSTATUS");
    bool is_connected = _last_status.charAt(_last_status.indexOf("STATUS:") + 7);

    if (is_connected)
        Serial.println("Already connected");
    
    return is_connected;
}

bool ESP::send_post_request(const String host, Reading *r)
{
    // Check if connection is still alive...
    if (!connect_host(host)) 
    {
        Serial.println("Can not connect to host");
        return false;
    } 
        
    // sensor id
    char id_sensor[3];
    dtostrf(r->id_sensor, 2, 0, id_sensor);
    
    strcpy (_post_data, "id_sensor=");
    strcat (_post_data, id_sensor);
    
    // reading value
    char reading[6]; 
    dtostrf(r->reading, 2, 2, reading);

    strcat (_post_data, "&value=");
    strcat (_post_data, reading);

    // timestamp
    strcat (_post_data, "&timestamp=");
    strcat (_post_data, r->timestamp);
    strcat (_post_data, "&user=aquarium_arduino&pwd=test");

    char post_data_l[6];
    dtostrf(strlen(_post_data), 2, 0, post_data_l);    

    strcpy (_command, "POST /aquarium_post.php HTTP/1.1\r\n");
    strcat (_command, "Host: 198.160.0.179\r\n");
    strcat (_command, "Content-length: ");
    strcat (_command, post_data_l);
    strcat (_command, "\r\n");
    strcat (_command, "Connection: Keep-Alive\r\n");
    strcat (_command, "User-Agent: Aquarium_Arduino\r\n");
    strcat (_command, "Content-type: application/x-www-form-urlencoded\r\n\r\n");
    strcat (_command, _post_data);

    Serial.println("Openning connection...");
    _self->print("AT+CIPSEND="); 
    _self->println(strlen(_command));
    delay(200); // wait for the response
    
    // check if ESP responded
    _last_status = _self->readString();
    if(_last_status.indexOf(">") > 0) {Serial.println("Sending data..."); } 
    else 
    {
        Serial.println("Data not sent.");
        Serial.println("Error data:");
        Serial.println(_last_status);

        execute_at("AT+CIPCLOSE"); // disconnect from the server

        return false;
    }

   _self->print(_command);
    delay(100);

    Serial.println("Data sent...");
    
    // free up the memory
    memset(_post_data, 0, sizeof(_post_data));
    memset(_command, 0, sizeof(_command));

    return true;
}

void ESP::_handle_err(const String *msg)
{
    Serial.println("AP connection error:");
    char errcode = msg->charAt(msg->indexOf("CWJAP:")+6);

    switch (errcode)
    {
    case '1':
        Serial.println("Timeout");
        break;
    case '2':
        Serial.println("Wrong password");
        break;
    case '3':
        Serial.println("SSID not found");
        break;
    case '4':
        Serial.println("Connection failed");
        break;
    default:
        Serial.println("Unknow error...");
        break;
    }

    return;
}

bool ESP::execute_at(const String cmd, const String escape, int i)
{
    if (i>=10) 
    {
        _last_status = "timeout";
        return false; 
    } // timeout

    _self->flush(); // clear buffer before seding command
    delay(100);

    // send command and wait for response
    _self->println(cmd); 
    while (_self->available() == 0) { delay(100); } // wait for response
    
    String response;
    response = _self->readString();
    _last_status = response;

    if (response.indexOf(escape) > 0) { return true; }
    while (response.indexOf("busy") > 0) // check if serial is busy
    {
        _self->flush(); // clear buffer before serial testing
        delay(100);

        _self->println("AT"); // check if ESP is ready for commands
        delay(100);
        response=_self->readString(); // read response
        if (response.indexOf("OK") > 0){break;} // break if serial is ready
    }

    return execute_at(cmd, escape, i+1);
}