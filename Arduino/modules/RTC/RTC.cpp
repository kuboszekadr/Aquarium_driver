#include "RTC.h"

RTC::RTC(int rts, int clk, int dat)
{
    _rts = rts;
    _clk = clk;
    _dat = dat;

    _RTCWire = new ThreeWire (_dat, _clk, _rts);
    _rtc = new RtcDS1302<ThreeWire> (*_RTCWire);
}

bool RTC::init() 
{
  _rtc->Begin(); //init RTC clock
  _rtc->SetIsWriteProtected(true); //make sure that clock is protected
  return _rtc->GetIsRunning();
}

void RTC::set_date(int year, int month, int day, int hour, int minute, int second)
{
    _rtc->SetIsWriteProtected(false);
    _rtc->SetDateTime(RtcDateTime(year, month, day, hour, minute, second));
    _rtc->SetIsWriteProtected(true);
}

void RTC::get_timestamp(char* timestamp)
{
    RtcDateTime now = _rtc->GetDateTime();
    sprintf(timestamp, "%d%02d%02d %02d%02d%02d", now.Year(), now.Month(), now.Day(), now.Hour(), now.Minute(), now.Second());
}
