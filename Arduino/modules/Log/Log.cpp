#include <Log.h>

static Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

void Logger::set_rtc()
{
    RTC& _rtc = RTC::get_instance();
}

void Logger::set_lcd(LiquidCrystal_I2C *lcd)
{
    Logger& logger = Logger::getInstance(); // get logger instance
    logger._lcd = lcd;
    logger._lcd->init();
    logger._lcd->backlight();
}

void Logger::set_log_level(LogLevel log_level)
{
    LogLevel _log_level = log_level;
}

void Logger::log(const char *msg)
{
    Logger& logger = getInstance(); // get logger instance
    clear(logger._timestamp); // clear previous timestamp

    logger._rtc.get_timestamp(logger._timestamp); // get current timestamp
    logger._log_serial(msg); // print msg
    logger._log_lcd(msg); // show msg on liquid crystal screen
    logger._log_sd(msg); // save log to the file
}

void Logger::log(const char *msg, const char* timestamp)
{
    Logger& logger = Logger::getInstance(); // get logger instance
    clear(logger._timestamp); // clear previous timestamp

    strcpy(logger._timestamp, timestamp); // copy timestamp value
    
    logger._log_serial(msg); // print msg
    logger._log_sd(msg); // save log to the file
    logger._log_lcd(msg); // show msg on liquid crystal screen
}

void Logger::_log_serial(const char *msg)
{
    Serial.print(_timestamp);
    Serial.print(": ");
    Serial.println(msg);
}

void Logger::_log_lcd(const char *msg)
{
    static char logs[4][50];

    Logger& logger = Logger::getInstance(); // get logger instance
    logger._lcd->clear();

    for (int i = 1; i < 4; i++)
    {
        logger._lcd->setCursor(0, i-1); 
        logger._lcd->print(logs[i]);

        memset(logs[i-1], 0, sizeof(logs[i-1]));   // clear data before offseting   
        strcpy(logs[i-1], logs[i]);  // offset logs by one    
    }
    
    memset(logs[3], 0, sizeof(logs[3]));   // clear data before offseting     
    strcpy(logs[3], msg); // insert new message

    logger._lcd->setCursor(0, 3);
    logger._lcd->print(logs[3]);
}

void Logger::_log_sd(const char *msg)
{
    Logger& logger = Logger::getInstance();
    memset(logger._log_file_name, 0, 20);

    //  create new file name YYYYMMDD.log
    substring(logger._log_file_name, logger._timestamp, 0, 8);  // take only YYYYMMDD info from the timestamp
    strcat(logger._log_file_name, ".txt");  

    logger._log_file = SD.open(logger._log_file_name, FILE_WRITE);

    if (logger._log_file)
    {
        logger._log_file.print(_timestamp);
        logger._log_file.print(": ");
        logger._log_file.println(msg);    
        logger._log_file.close();
    }
    else 
        _log_serial("Can not open the file.");
}

void Logger::log(bool value)
{
    char val[2];
    val[0] = value ? '1' : '0';

    log(val);
}