#ifndef Log_h
#define  Log_h

#include <SoftwareSerial.h>
#include <Utils.h>

#include <RTC.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <SD.h>

enum LogLevel {APPLICATION = 0,  // print only data from the main 
                ERROR,  // show errors
                WARNING,   // warnings
                VERBOSE};  // all messages

class Logger
{   
    public:    
        static Logger& getInstance();
        static void log(const char *msg);
        static void log(bool value);
        static void log(const char *msg, const char *timestamp);
        
        static void set_rtc();
        static void set_log_level(LogLevel log_level);
        static void set_lcd(LiquidCrystal_I2C *lcd);
    
    private:
        Logger() {};
        Logger(const Logger&) {};

        // void _log_sd();
        void _log_serial(const char *msg);
        void _log_lcd(const char *msg);
        void _log_sd(const char *msg);

        LiquidCrystal_I2C *_lcd;
        RTC& _rtc;

        char _log_file_name[20];
        File _log_file;

        char _timestamp[17];
        LogLevel _log_level;

};

#endif