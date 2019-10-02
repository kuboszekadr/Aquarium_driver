#include "WaterLevel.h"

WaterLevel::WaterLevel(int echo, int trig, int id_sensor)
{
	_echo = echo;
	_trig = trig;
	_id_sensor = id_sensor;
	
	pinMode(_trig, OUTPUT);
	pinMode(_echo, INPUT);
}

int WaterLevel::id_sensor()
{
	return _id_sensor;
}

float WaterLevel::get_water_level()
{
	// returns water level in cm
	digitalWrite(_trig, LOW);
	delayMicroseconds(2);
	
	digitalWrite(_trig, HIGH);
	delayMicroseconds(10);
	
	// Measuring distance
	digitalWrite(_trig, LOW);

	// count how many readings are available in the array
	_readings_count = _readings_count < READINGS_SIZE ? _readings_count + 1 : READINGS_SIZE;
	_reading = (_reading + 1) % READINGS_SIZE; // overwrite the array if needed
	_readings[_reading] = pulseIn(_echo, HIGH) / 58.0; // insert new reading data

	avg();
	return _readings[_reading];
}

WaterStatus WaterLevel::get_water_status()
{
	Serial.println(_avg);
	if (_avg < UPPER_LEVEL)
		return OVERFILLED;
	else if (_avg > LOWER_LEVEL)
		return LACKING;
	else
		return OK;		
}

void WaterLevel::avg()
{
	// calcualtes moving average only for those readings which 
	float _sum = 0.0;
	for (int i=0; i < _readings_count; i++)
		_sum += _readings[i];

	_avg = _sum * 1.0 / _readings_count;
	Serial.println(_avg);
}