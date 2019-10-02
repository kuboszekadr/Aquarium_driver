#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DS18B20.h>

#include <ESP_8266.h>
#include <PhMeter.h>
#include <ReadingsQueue.h>

#include <RTC.h>
#include <WaterLevel.h>

#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// ESP configuration data
String HOST= "192.168.0.179";
String SSID = "Zdrajcy metalu";
String PWD = "Dz3nt31m3n_m3ta1u";

WaterLevel water_level_sump(6, 7, 2);

ESP esp(13, 12); // rx, tx
RTC rtc(5, 3, 4); // 
PhMeter ph_meter(A0, 3);

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

    digitalWrite(30, HIGH);
    delay(1000);

    digitalWrite(30, LOW);
    delay(1000);


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
    get_temp();
    get_water_level();        
    get_ph();
    send_data();

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

void log(Reading *r, const char *display_name)
{
    char _reading[6];
    dtostrf(r->value, 2, 2, _reading);

    char _log[20];
    strcpy(_log, display_name);
    strcat(_log, _reading);

    log(r->timestamp);
    log(_log);

    memset(_log, 0, sizeof(_log));
    memset(_reading, 0, sizeof(_reading));
}

void get_temp()
{
    if (!sensors.available())
        return;

    // create new reading record
    struct Reading reading;
    reading.id_sensor = 1;
    reading.value = sensors.readTemperature(address);;        
    rtc.get_timestamp(reading.timestamp); // get timestamp        
    
    (void) readings.add(&reading); 
    log(&reading, "Temp:");
}

void get_water_level()
{
    // create new reading record
    struct Reading reading;
    reading.id_sensor = water_level_sump.id_sensor();
    reading.value = water_level_sump.get_water_level();
    rtc.get_timestamp(reading.timestamp); // get timestamp

    (void) readings.add(&reading);
    log(&reading, "WL S:");
}

void get_ph()
{
    struct Reading reading;
    reading.id_sensor = ph_meter.get_id_sensor();
    reading.value = ph_meter.get_ph();
    rtc.get_timestamp(reading.timestamp); // get timestamp

    (void) readings.add(&reading);
    log(&reading, "Ph:");
}

void send_data()
{
    esp.send_post_request(HOST, &readings);
}
