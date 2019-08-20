#include "Arduino.h"
#include "WaterLevel.h"

WaterLevel::WaterLevel(int echo, int trig){
	_echo = echo;
	_trig = trig;
	
	pinMode(_trig, OUTPUT);
	pinMode(_echo, INPUT);
}

double WaterLevel::get_water_level(){
	// returns water level in cm
	digitalWrite(_trig, LOW);
	delayMicroseconds(2);
	
	digitalWrite(_trig, HIGH);
	delayMicroseconds(10);
	
	// Measuring distance
	digitalWrite(_trig, LOW);
	return pulseIn(_echo, HIGH) / 58.0;
}