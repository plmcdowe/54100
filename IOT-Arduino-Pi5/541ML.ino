#include <WiFi.h>
#include <Adafruit_MLX90614.h>
#include "arduino_secrets.h"

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
int status = WL_IDLE_STATUS;
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
char server[] = "192.168.1.104"; //server ip

WiFiClient client;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ;
  }

  //begin connect WiFi:
  while (status != WL_CONNECTED) {
    Serial.print("attempting connection to: ");
    Serial.println(ssid);
    //WPA/2 network
    status = WiFi.begin(ssid, pass);

    delay(3000);
  }
  Serial.println("pulling internet from the sky");
  printWifiStatus();

  mlx.begin(); //start the sensors
}

void loop() {
  WiFiClient client;

  float ambient = 0;
  float object = 0;
  
  //debug the raw sensor readings
  ambient = mlx.readAmbientTempF();
  Serial.println(ambient);
  object = mlx.readObjectTempF();
  Serial.println(object);

  //stringify for concatenation in jsonData 
  String ambientStr = String(ambient, 1);
  String objectStr = String(object, 1);

  String jsonData = "{\"ambientTemp\":"+ ambientStr +",\"objectTemp\":"+ objectStr +"}";
  Serial.println(jsonData); // debug format of json data string
  size_t jsonLen = jsonData.length(); //get length of json data string as unsigned int

  //using port 57391
  if (client.connect(server, 57391)) {
    Serial.println("conn success");

    client.println("POST /Temp.php HTTP/1.1"); 
    client.println("Host: 192.168.1.104"); //server ip
    client.println("Content-Type: application/json");
    client.print("Content-Length: "); //no `println`, the length below must be on same line
    client.println(jsonLen); //length of json data being sent
    client.println();//empty line = end-of-header
    client.print(jsonData);//sends json

    //timeout-wait for server response
    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 500) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
        Serial.println(line); //print server response
        timeout = millis();
      }
    }

    client.stop(); //close connection
    Serial.println("conn closed");
  } else {
    Serial.println("conn fail");
  }  
  delay(1000);

}

//upon connection: function to return SSID, board's IP, and signal strength
void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}