#include "Arduino.h"
#include "SoftwareSerial.h"
#include "ESP.h"

ESP::ESP (int rx_pin, int tx_pin) {
	_self = new SoftwareSerial  (rx_pin, tx_pin);
	_self->begin(9600);
}

//Restarts ESP module and configures is as needed
void ESP::restart() {
	
	// Restart the module
	(void) execute_cmd("AT+RST");
	
	// Set ESP as client
	(void) execute_cmd("AT+CWMODE=1");
	
	// Setting receive mode
	(void) execute_cmd("AT+CIPMODE=0");
	
	// Allow multiply connections
	(void) execute_cmd("AT+CIPMUX=1");	
}

void ESP::connect_to(String ssid, String pwd) {
	(void) execute_cmd("AT+CWJAP=\"Zdrajcy metalu\",\"Dz3nt31m3n_m3ta1u\"");
}

void ESP::start_server(){
	(void) execute_cmd("AT+CIPSERVER=1,80");
}

String ESP::get_ip_address(){
	if (_ip != "") { return _ip;}

	String response = execute_cmd("AT+CIPSTA?");

	if (response.indexOf("+CIPSTA") == -1) {
		return "ERROR\n" + response;
	}

	_ip = response;
//	_ip = response.substring(response.indexOf("ip:")+1, 20);//response.indexOf("\n"));
	return _ip;
}
/*
bool ESP::client_available(){
	String r = _self->readString();
	
	if (r.indexOf("CONNECT") != -1){ return true;}
	else { return false;}
}
*/

String ESP::client_available(){
	return _self->readString();	
}

void ESP::send_data(String *data){
	//send data to the client
	_self->print("AT+CIPSEND=0,");
	_self->println(data->length());
	
	delay(100);
	
	// Sending the request data
	if (_self->find(">")){
		Serial.println("Sending data...");
		_self->println(*data);
	}
	delay(100);
	
	// Check if data has been sent
	if (_self->find("SEND OK")) {Serial.println("Data sent.");}

	_self->println("AT+CIPCLOSE=0");  // closes client the connection
}

String ESP::execute_cmd(String cmd) {
	_self->println(cmd);
	delay(1000);
	String response = _self->readString();
	delay(500);
/*	
	//Loop if response is not correct
	int i = 0;
	while (response.indexOf("OK") == -1 & i < 10) {
		_self->println(cmd);
		delay(500);
		response = _self->readString();
		i ++;  
	}
*/
	return response;
}