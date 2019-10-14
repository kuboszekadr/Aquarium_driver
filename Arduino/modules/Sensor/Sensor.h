#ifndef Sensor_h
#define  Sensor_h

#include "Arduino.h"
#include "RTC.h"
#include "Utils.h"

// send new data in approx every 30s
#define SAMPLING_INTERVAL 1000L // sample every second
#define SAMPLING_AMOUNT 30  // readings array size

struct Reading
{
  int id_sensor;
  float value;
  char timestamp[17];
};

class Sensor
{
    public:
        virtual bool make_reading() = 0;

        Reading get_reading();

        bool available();
        bool ready();

    protected:
        float _avg();
        float _last_reading_value;

        float _readings[SAMPLING_AMOUNT];
        unsigned long _last_reading = 0;

        int _readings_count = -1;
        int _id_sensor = -1;
};

#endif  