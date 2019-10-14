// Communication
#include <OneWire.h>
#include <ThreeWire.h>
#include <Wire.h> 
#include <ESP_8266.h>
#include <Adafruit_NeoPixel.h>
#include <SPI.h>
#include <SD.h>

// Displays
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

// Sensors
#include <PhMeter.h>
#include <WaterLevel.h>
#include <Thermometer.h>

// Helpers
#include <Utils.h>
#include <Log.h>
#include <RTC.h>
#include <ReadingsQueue.h>

// ESP configuration data
char HOST[14] = "192.168.0.179";
char SSID[20] = "Zdrajcy metalu";
char PWD[20] = "Dz3nt31m3n_m3ta1u";

ESP esp(13, 12); // rx, tx

/* SENSORS */
PhMeter ph_meter(A0, 3);
WaterLevel water_level_sump(6, 7, 2);

// DS18B20 - Thermometer
char address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
Thermometer thermometer(2, 1, address);

/* OTHER */
ReadingsQueue readings;  // to hold all readings to be sent
char msg[30];  // to create log messages

// Pixels
// Adafruit_NeoPixel lamp_rigth = Adafruit_NeoPixel(200, 40, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel lamp_middle = Adafruit_NeoPixel(200, 40, NEO_GRB + NEO_KHZ800);
// Adafruit_NeoPixel lamp_left = Adafruit_NeoPixel(200, 40, NEO_GRB + NEO_KHZ800);

void setup()
{
    delay(2000); // wait for boot
    Serial.begin(9600);  // available serial output

    SD.begin(53);  // start SD card

    RTC& rtc = RTC::get_instance(); 
    rtc.init(5, 3, 4);  // init RTC

    LiquidCrystal_I2C *lcd = new LiquidCrystal_I2C(0x27,20,4);
    Logger::set_lcd(lcd);

    Logger::log("ESP - ready");
    Logger::log(esp.connected());

    Logger::log("ESP - restarting");
    Logger::log(esp.restart());

    Logger::log("ESP - WiFi connection");
    Logger::log(esp.connect_wifi(SSID, PWD));

    Logger::log("Setup complete");
}

void loop()
{
    check_water_level();
    read_temperature();
    send_data();

    delay(1000);
}

void check_water_level()
{
    // check if sensor is ready
    if(water_level_sump.ready())
    {
        Logger::log("Getting water level");
        bool is_read = water_level_sump.make_reading();
        is_read ? Logger::log("OK") : Logger::log("Ups...");
    }
    else
        Logger::log("Water level sensor not ready yet");
    
    // check if queue is full
    if(water_level_sump.available())
    {
        // get average water level
        struct Reading r = water_level_sump.get_reading();
        readings.add(&r);

        // show water level
        char water_level[6];
        dtostrf(r.value, 2, 2, water_level);

        clear(msg); // clear previous message
        strcpy(msg, "Water level: ");
        strcat(msg, water_level);
        Logger::log(msg);  // log water level

        // check status
        clear(msg);  // clear previous message  

        WaterStatus water_status = water_level_sump.get_water_status(); 
        sprintf(msg, "Water status: %d", water_status);  
        Logger::log(msg);  // log water status

        switch (water_status)
        {
        case -1:
            // start filling
            break;
        case 0:
            // stop filling
            break;
        case 1:
            break;
        default:
            break;
        }
    }
}

void read_temperature()
{
    if (thermometer.ready())
    {
        Logger::log("Getting temperature");
        bool is_read = thermometer.make_reading();  // make reading
        is_read ? Logger::log("OK") : Logger::log("Ups...");
    }
    else
        Logger::log("Thermometer not ready yet");

    if (thermometer.available())
    {
         // get average temperature
        struct Reading r = thermometer.get_reading();
        readings.add(&r);

        // show water level
        char temp[6];
        dtostrf(r.value, 2, 2, temp);

        clear(msg); // clear previous message
        strcpy(msg, "Temp: ");
        strcat(msg, temp);
        Logger::log(msg);  // log water level       
    }
}

void send_data()
{
    if (!readings.is_empty())
    {
        Logger::log("Sending data...");
        bool is_sent = esp.send_post_request(HOST, readings);

        if (is_sent)
            Logger::log("Data sent.");
        else
            Logger::log("Failure.");
    }
    else
        Logger::log("Queue empty");
}
