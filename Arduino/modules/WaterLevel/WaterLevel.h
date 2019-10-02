/*
  WaterLevel.h - wraps HC-SR04 sensor to get water level
  Created by Adrian Kuboszek adrian.kuboszek@gmail.com 04.05.2019
  Last update: 
*/

#ifndef WaterLevel_h
#define  WaterLevel_h

#include "Arduino.h"

#define READINGS_SIZE 10
#define UPPER_LEVEL 5.0
#define LOWER_LEVEL 15.0

#define SAMPLING_INTERVAL 1000

enum WaterStatus {LACKING = -1, OK, OVERFILLED, FILLING, PUMPING_OUT};

class WaterLevel 
{
	public:
		WaterLevel(int echo, int trig, int id_sensor);
		float get_water_level();
		int id_sensor();

		WaterStatus get_water_status();

	private:
		int _echo;
		int _trig;
		int _id_sensor;

		int _reading = -1;
		int _readings_count = 0;
		float _readings[READINGS_SIZE];

		void avg();
		float _avg;
};

#endif
