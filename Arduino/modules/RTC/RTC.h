#ifndef RTC_h
#define RTC_h

#include <Arduino.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>

class RTC
{
    public:
        RTC(int rts, int clk, int dat);

        bool init();
        void set_date(int year, int month, int day, int hour, int minute, int second);
        void get_timestamp(char *timestamp);

    private:
        int _rts;
        int _clk;
        int _dat;

        ThreeWire *_RTCWire;
        RtcDS1302<ThreeWire> *_rtc;
};

#endif