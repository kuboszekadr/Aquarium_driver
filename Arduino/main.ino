#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DS18B20.h>

#include <ESP_8266.h>
#include <ReadingsQueue.h>

#include <RTC.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// ESP configuration data
String HOST= "192.168.0.179";
String SSID = "Zdrajcy metalu";
String PWD = "Dz3nt31m3n_m3ta1u";

ESP esp(13, 12); // rx, tx
RTC rtc(5, 3, 4); // 

ReadingsQueue readings;
Reading reading;

// DS18B20
byte address[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
OneWire onewire(2);
DS18B20 sensors(&onewire);

// LCD
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display

char logs[4][20];

void setup()
{
    Serial.begin(9600);

    lcd.init(); 
    lcd.backlight();

    log("RTC connection");
    log(rtc.init()?"1":"0");
    // rtc.set_date(2019, 10, 1, 10, 21, 0);

    log("ESP");
    log("Connection");
    log(esp.connected()?"1":"0");

    log("Restarting");
    log(esp.restart()?"1":"0");

    log("WiFi connection");
    log(esp.connect_wifi(&SSID, &PWD)?"1":"0");

    log("Setup complete");

    sensors.begin(12);
    sensors.request(address);
}

void loop()
{
    if(sensors.available())
        get_temp();

    while (!readings.is_empty())
    {
        Reading r = readings.pop();
        // log("Sending data to the server...");
        esp.send_post_request(HOST, &r);

    }

    Serial.println("availableMemory:" + (String) availableMemory());
    delay(500);
}

int availableMemory()
{
  int size = 8192;
  byte *buf;
  while ((buf = (byte *) malloc(--size)) == NULL);
  free(buf);
  return size;
}

void log(const char *msg)
{
    Serial.println(msg);
    lcd.clear();

    for (int i = 1; i < 4; i++)
    {
        lcd.setCursor(0, i-1); 
        lcd.print(logs[i]);

        memset(logs[i-1], 0, sizeof(logs[i-1]));   // clear data before offseting     
        strcpy(logs[i-1], logs[i]); // offset logs by one
    }

    memset(logs[3], 0, sizeof(logs[3]));   // clear data before offseting     
    strcpy(logs[3], msg); // insert new message
    
    lcd.setCursor(0, 3);
    lcd.print(logs[3]);
}

void get_temp()
{
    struct Reading reading;
    reading.id_sensor = 1;
    reading.reading = sensors.readTemperature(address);;        
    rtc.get_timestamp(reading.timestamp); // get timestamp        

    sensors.request(address);
    (void) readings.add(&reading);

    char _temp[6];
    dtostrf(reading.reading, 2, 2, _temp);

    char _log[13] = "Temp: ";
    strcat(_log, _temp);

    log(reading.timestamp);
    log(_log);

    memset(_log, 0, sizeof(_log));
    memset(_temp, 0, sizeof(_temp));
}