#include <OneWire.h> //helper for thermometer
#include <ThreeWire.h> //helper for RTC

#include <RtcDS1302.h> //RTC 
#include <DS18B20.h> //termometr 
#include <SoftwareSerial.h>

/*
 * PINS DEFINITIONS
 */
//RTC
#define RTC_CE 3 // RST
#define RTC_SCLK 4 // CLK
#define RTC_IO 5 // DAT

#define THERMOMETER 6

#define SUMP_ECHO 7
#define SUMP_TRIG 8

#define AQUARIUM_ECHO 9
#define AQUARIUM_TRIG 10

#define ESP_RX 12
#define ESP_TX 13

// Others
#define ESP_Banwidth 9600
#define ESP_TargetNetworkSSD "TP-LINK_BE0B92" //name of network to be connected to
#define ESP_TargetNetworkPWD "5krokusowa5" //target network password

ThreeWire RTCWire(RTC_IO, RTC_SCLK, RTC_CE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(RTCWire); // initalizes RTC
SoftwareSerial ESP(ESP_RX, ESP_TX);

byte ThermoAddress[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
OneWire ThermoWire(THERMOMETER);
DS18B20 Thermo(&ThermoWire);

void setup() {
  Serial.begin(9600);

  Serial.println("Launching WiFi...");
  init_ESP();

  Serial.println("Lauching RTC...");
  init_RTC();

  Serial.println("Launching termo...");
  init_Thermo();

  Serial.println("Launing HCSR...");
  init_HCSR();

  Serial.println("Setup finished.");
  Serial.println("");
}

void loop() {
  String data = timestamp();
  
  data += "\tTemp:";
  data += get_temperature();
  
  data += "\tSumpLevel:";
  data += get_water_level();
  
  Serial.println(data);

  String r = ESP.readString();
  Serial.println(r);

  if (r.indexOf("CONNECT") != -1) {
    Serial.println("New connection available...\nSending data...");
    ESP.println("AT+CIPSEND=0," + String(data.length()));
    delay(100);
    
    if (ESP.find(">")) {Serial.println("Sending..."); ESP.print(data);};
    if (ESP.find("SEND OK")) {Serial.println("Packet sent");};
  
    ESP.println("AT+CIPCLOSE=0");
    delay(500);
    Serial.println(ESP.readString());
  }

  delay(500);

}

// inits ESP and connects to target network 
void init_ESP() {
  ESP.begin(9600);

  Serial.println("Restarting module...");
  send_esp_command("AT+RST");  
  send_esp_command("AT+CWMODE=1");
  send_esp_command("AT+CIPMODE=0");
  send_esp_command("AT+CIPMUX=1");

  Serial.println("Connecting to target network...");
  send_esp_command("AT+CWJAP=\"TP-LINK_BE0B92\",\"5krokusowa5\""); 

  Serial.println("Getting IP address...");
  retreive_esp_command("AT+CIPSTA?");

  send_esp_command("AT+CIPSERVER=1,80");
}

void retreive_esp_command(String cmd) {
  ESP.println(cmd);
  String r = ESP.readString();

  while (r.indexOf("OK") == 0) {
    ESP.println(cmd);
    delay(500);
    r = ESP.readString();
  }
  Serial.println(r);
}

void send_esp_command(String cmd){
  //ESP.readString();
  ESP.println(cmd);
  
  while (ESP.find("OK") == 0) {
    ESP.println(cmd);
    delay(500);
  }
}

void init_RTC() {
  Rtc.Begin(); //init RTC clock

  if (Rtc.GetIsRunning()) { //check if clock is running
    Serial.println("Clock is working");
  }
  else {
    Serial.println("Clock is not working!");
  }

  Rtc.SetIsWriteProtected(true); //make sure that clock is protected
}

//Returns current timestamp
String timestamp() {
  RtcDateTime dt = Rtc.GetDateTime();
  
  int year = dt.Year();
  int month = dt.Month();
  int day = dt.Day();

  int hour = dt.Hour();
  int minute = dt.Minute();
  int second = dt.Second();

  String result = (String) year;
  result += print_2digits(month);
  result += print_2digits(day);
  result += print_2digits(hour);
  result += print_2digits(minute);
  result += print_2digits(second);

  return result;
}

void init_Thermo() {
  Thermo.begin();
  Thermo.request(ThermoAddress);  
}

//Inits HCSR to get water level in each section
void init_HCSR() {
  pinMode(SUMP_TRIG, OUTPUT);
  pinMode(SUMP_ECHO, INPUT);

  pinMode(AQUARIUM_TRIG, OUTPUT);
  pinMode(AQUARIUM_ECHO, INPUT);
}

float get_water_level() {
  digitalWrite(SUMP_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(SUMP_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(SUMP_TRIG, LOW);
 
  return pulseIn(SUMP_ECHO, HIGH) / 58.0;
}
//Reads data from thermometer in Celsius
float get_temperature() {
  if (Thermo.available()) {
    float temp = Thermo.readTemperature(ThermoAddress);
    Thermo.request(ThermoAddress);

    return temp;
  }
  return -1;
}

//Converts single digit number to double digit
//f.e 9 -> 09, 10 -> 10, 0 -> 00
String print_2digits(int value) {
  if (value < 10) { 
    return '0' + (String) value;
  }
  return (String) value;
}

