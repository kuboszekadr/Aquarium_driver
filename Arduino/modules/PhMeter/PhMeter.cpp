#include "PhMeter.h"

PhMeter::PhMeter(int pin, int id_sensor)
{
    _pin = pin;
    _id_sensor = id_sensor;
}


int PhMeter::get_id_sensor()
{
    return _id_sensor;
}

float PhMeter::get_ph()
{
    float voltage = analogRead(_pin);
    return voltage*5.0/1024;
}