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

#define ESP_RX 7
#define ESP_TX 8

// Others
#define ESP_Banwidth 1
#define ESP_TargetNetworkSSD "Zdrajcy metalu" //name of network to be connected to
#define ESP_TargetNetworkPWD "Dz3nt31m3n_m3ta1u" //target network password

ThreeWire RTCWire(RTC_IO, RTC_SCLK, RTC_CE); // IO, SCLK, CE
RtcDS1302<ThreeWire> Rtc(RTCWire); // initalizes RTC
SoftwareSerial ESP(ESP_RX, ESP_TX);

byte ThermoAddress[8] = {0x28, 0x25, 0x34, 0xE5, 0x8, 0x0, 0x0, 0x35};
OneWire ThermoWire(THERMOMETER);
DS18B20 Thermo(&ThermoWire);

void setup() {
  Serial.begin(9600);

  init_ESP();
  init_RTC();
  init_Thermo();

}

void loop() {
  Serial.print(timestamp());
  Serial.print(get_temperature());
}

// inits ESP and connects to target network 
void init_ESP() {
  
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

