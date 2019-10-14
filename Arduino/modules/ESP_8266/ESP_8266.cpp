#include "ESP_8266.h"

ESP::ESP(int rx_pin,  int tx_pin)
{
    _self = new SoftwareSerial (rx_pin, tx_pin);
    _self->begin(9600);
}

bool ESP::connected()
{
    return execute_at("AT");  // test ESP response
}

bool ESP::restart()
{
    bool rst = execute_at("AT+RST");  // restart ESP  
    delay(2000);  // wait for reastart
    clear_buffer(_self);  // clean leftovers

    (void) execute_at("ATE0");  // turn the echo off
    bool mode = execute_at("AT+CIPMODE=0");  // set ESP in normal transmission mode

    return (rst && mode);
}

bool ESP::execute_at(const char *cmd)
{
    return execute_at(cmd, "OK");
}

bool ESP::connect_wifi(const char *ssid, const char *pwd)
{
    sprintf(_command, "AT+CWJAP=\"%s\",\"%s\"", ssid, pwd);  // generate AT command
    print_serial(_command, _self);  // send to the buffer
    
    int i = 0;
    _is_connected = false;  // set ESP as not connected
    
    while (!_is_connected)
    {
        // check if new data is available
        if(_self->available() > 0)
        {              
            read_serial(_last_status, 256, _self);

            // check if response is an error
            if(strstr(_last_status, "FAIL")) 
            {
                Logger::log(_last_status);  // show the error message
//                _handle_err(&resp); // parse response to check error
               break;
            }
            else if (strstr(_last_status, "OK"))
            {
                char msg[100];
                sprintf(msg, "Connected to: %s", ssid);
                Logger::log(msg);

                clear_buffer(_self);
                _is_connected = true;
            }
        }
        else    
            delay(500);  // wait for new data

        if (i == 50)    
            Logger::log("Connection takes longer then expected...");
        if (i >= 100)
        {
            Logger::log("Connection timeout");
            break;
        }
        i++;
    }

    clear_buffer(_self);
    return _is_connected;
}

bool ESP::connect_host(const char *host)
{
    char status = connection_status();  // get connection status

    // check if connection is not open
    if (status == '3') 
        return true;  // return true if connection is alive

    // otherwise open new connection
    clear(_command);  // clear previous command
    sprintf(_command, "AT+CIPSTART=\"TCP\",\"%s\",80", host);

    return execute_at(_command); 
}

char ESP::connection_status()
{ 
    (void) execute_at("AT+CIPSTATUS"); // get current connection status   
    return _last_status[7]; // return status char
}

bool ESP::send_post_request(const char *host, const ReadingsQueue &queue)
{
    Logger::log("Connecting to the host...");

    // Check if connection is still alive...
    if (!connect_host(host)) 
    {
        Logger::log("Can not connect to host");
        return false;
    } 

    Logger::log("Creating post request string...");
    queue.generate_post_request(_post_data, ";");  // generate post data string
    strcat (_post_data, "&user=aquarium_arduino&pwd=test");  // add user and pwd 

    clear (_command);
    sprintf(_command, 
            "POST /aquarium_post.php HTTP/1.1\r\n" \
            "Host: 198.160.0.179\r\n" \
            "Content-length: %d\r\n" \
            "Connection: Keep-Alive\r\n" \
            "User-Agent: Aquarium_Arduino\r\n" \
            "Content-type: application/x-www-form-urlencoded\r\n\r\n" \
            "%s", strlen(_post_data), _post_data);
    
    // Logger::log(_command);
    Logger::log("Openning connection...");

    // I want to have post request data ready to send
    // thus i am sending AT command manually
    _self->print("AT+CIPSEND="); 
    _self->println(strlen(_command));
    _self->flush();  // wait for data transfer
    delay(500);  // wait for the response

    Logger::log("Reading response from the ESP...");
    read_serial(_last_status, 256, _self);  // get ESP response

    if (strstr(_last_status, ">")) // response as expected
        Logger::log("Sending data..."); 
    else // ups...
    {
        Logger::log("Sending error, ESP response below:");
        Logger::log(_last_status);

        // clear the data
        clear(_post_data);
        clear(_command);
        clear_buffer(_self);
        delay(500);
       (void) execute_at("AT+CIPCLOSE");  // disconnect from the server
        return false;
    }

    _self->print(_command);  // sending post request
    _self->flush();  // wait for transfer

    Logger::log("Post request data sent...");
    Logger::log("Waiting for feedback.");
    delay(1000);  // wait to read full response

    Logger::log("ESP response:");
    read_serial(_last_status, 256, _self);
    Logger::log(_last_status);

    // clear the data
    clear(_post_data);
    clear(_command);
    clear_buffer(_self);

    return true;
}

// void ESP::_handle_err(const String *msg)
// {
//     Serial.println("AP connection error:");
//     char errcode = msg->charAt(msg->indexOf("CWJAP:")+6);

//     switch (errcode)
//     {
//     case '1':
//         Serial.println("Timeout");
//         break;
//     case '2':
//         Serial.println("Wrong password");
//         break;
//     case '3':
//         Serial.println("SSID not found");
//         break;
//     case '4':
//         Serial.println("Connection failed");
//         break;
//     default:
//         Serial.println("Unknow error...");
//         break;
//     }

//     return;
// }

bool ESP::execute_at(const char *cmd, const char *escape)
{
    int i = 0;
    bool status = false;

    while (!status & i < TIMEOUT)
    {
        i++;
        status = _execute_at(cmd, escape);
        delay(500);
    }
    
    if (i==10)
    {
        Logger::log("Connection timeout. ESP response:");
        Logger::log(_last_status);
    }

    clear_buffer(_self);  // remove leftovers
    return status;
}

bool ESP::_execute_at(const char *cmd, const char *escape)
{
    // send command and wait for response
    print_serial(cmd, _self); 
    
    // wait for response
    while (_self->available() == 0) 
        delay(100); 
    
    // get response
    read_serial(_last_status, 256, _self); 

    // check if ESP response is OK
    if (strstr(_last_status, escape)) 
        return true;
    
    // check if serial is busy if yes, wait
    while (strstr(_last_status, "busy")) 
    {
        print_serial("AT", _self);
        read_serial(_last_status, 256, _self);
        
        if (strstr(_last_status, "OK"))
            break; // break if serial is ready

        Logger::log("Waiting for serial to free...");
        delay(1000);
    }
    return false; // force next command execution
}
