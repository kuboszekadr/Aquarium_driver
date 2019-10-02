#ifndef PhMeter_h
#define  PhMeter_h

#define SAMPLING_INTERVAL 1000

#include "Arduino.h"

class PhMeter
{
    public:
        PhMeter(int pin, int id_sensor);
        
        float get_ph();
        int get_id_sensor();
    
    private:
        int _pin;
        int _id_sensor;
};

#endif  