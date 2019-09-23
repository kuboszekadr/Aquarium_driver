#include <ESP_8266.h>
#include <SoftwareSerial.h>

ESP esp(13, 12); //rx, tx
String HOST= "";

void setup()
{
    Serial.begin(9600);
    Serial.println("ESP connection");
    Serial.println(esp.connected());

    Serial.println("Restarting ESP...");
    Serial.println(esp.restart());

    Serial.println("Connecting to target WiFi...");
    Serial.println(esp.connect_wifi("Zdrajcy metalu", "Dz3nt31m3n_m3ta1u"));

    Serial.println("Setup completed.");
    Serial.println("....");

    HOST = "192.168.0.179";
}

void loop()
{
    esp.send_post_request(&HOST);
    delay(1000);
}
