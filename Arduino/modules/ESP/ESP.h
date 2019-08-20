/*
  ESP.h - library that wraps AT command necessary to manage 
  Aquarium driver using WiFI
  Created by Adrian Kuboszek adrian.kuboszek@gmail.com 03.05.2019
 */

#ifndef ESP_h
#define ESP_h

#include "Arduino.h"
#include "SoftwareSerial.h"

class ESP {
  public:
    ESP(int rx_pin, int tx_pin);
    
    void restart();
    void connect_to(String ssid, String pwd);
	void start_server();
	void send_data(String *data);
	
	String get_ip_address();
	
	String client_available();
	
  private:
    String _ip = "";
    String _ssid;
	
	SoftwareSerial *_self;

    String execute_cmd(String cmd);
};

#endif
