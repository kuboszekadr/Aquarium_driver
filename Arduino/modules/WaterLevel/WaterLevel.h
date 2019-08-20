/*
  WaterLevel.h - wrapt HC-SR04 sensor to get water level in different places
  Created by Adrian Kuboszek adrian.kuboszek@gmail.com 04.05.2019
  Last upate: 04.05.2019
 */

 #ifndef WaterLevel_h
#define  WaterLevel_h

#include "Arduino.h"

class WaterLevel {
	public:
		WaterLevel(int echo, int trig);
		double get_water_level();
		
	private:
		int _echo;
		int _trig;
};

#endif
