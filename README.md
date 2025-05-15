# Problem Statement:
***Extend SQLite syntax to incorporate at least one ML predicate with open-source ML libraries.***     

<b><ins>There are two efforts, broke into the separate directories</ins>:</b>    
### 1. 📂 [SQLite-ML-Files](https://github.com/plmcdowe/54100/tree/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files)
   - This directory contains all files necessary to extend SQLite syntax for:    
     numerical outlier detection and textual sentiment analysis.
   - SQLite-ML-Files is documented in this README first because it is the primary effort.
   - There are two implementation guides:
       1. Ubuntu desktop on a physical host
       2. Virtualized Ubuntu desktop in Oracle Box.
   - Shout out to 
### 2. 📂 [IOT-Arduino-Pi5](https://github.com/plmcdowe/54100/tree/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5)
   - This directory contains all files necessary to run SQLite-ML on a Raspberry Pi 5 with data from an Arduino Giga R1.
   - IOT-Arduino-Pi5 is documented after SQLite-ML-Files.

# \[ 1 \] SQLite-ML-Files:
> #### [ml_extension.c](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/ml_extension.c)    
> #### [ml_module.py](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/ml_module.py)     
> #### [shell.c](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/shell.c)     
> #### [sqlite3](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3)     
> #### [sqlite3.h](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3.h)     
> #### [sqlite3ext.h](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3ext.h)
> 
> ## \[ 1.1 \] <ins>Local Ubuntu 24 Desktop with Python venv</ins>:
>> **Dell laptop booting Ubuntu Desktop from external NVMe drive**
>>> ![NVMe](https://github.com/user-attachments/assets/7c7be5b9-b64e-4072-be0e-7bb4b3d6a8f1)
>>> **Python 3.12.3**    
>>> **Python 3-venv**    
>>> **joblib**    
>>> **numpy**    
>>> **scipy**    
>>> **pandas**    
>>> **transformers**    
>>> **scikit-learn**    
>>> **tensorflow**    
>
> ## \[ 1.2 \] Ubuntu VM on Windows 11 in OracleBox
>> **<ins>Windows 11 laptop running...</ins>:**     
>>> **Python 3.12.3**    
>>> **Python 3-venv**    
>>> **joblib**    
>>> **numpy**    
>>> **scipy**    
>>> **pandas**    
>>> **transformers**    
>>> **scikit-learn**    
>>> **tensorflow**    
>
# \[ 2 \] IOT-Arduino-Pi5:
> #### [541ML.ino](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/541ML.ino)      
> **Connects the Arduino Giga R1 to Wifi**    
> **Reads ambient and object temperatures**    
> **Packs the temperatures as JSON**    
> **Connects to the Raspberry Pi web server and publishes JSON formatted temperature readings to PHP script on Pi**    
>> ```C++
>> #include <WiFi.h>
>> #include <Adafruit_MLX90614.h>
>> #include "arduino_secrets.h"
>> 
>> Adafruit_MLX90614 mlx = Adafruit_MLX90614();
>> int status = WL_IDLE_STATUS;
>> char ssid[] = SECRET_SSID; //sourced from "arduino_secrets.h"
>> char pass[] = SECRET_PASS; //sourced from "arduino_secrets.h"
>> char server[] = "192.168.1.104"; //server ip
>> WiFiClient client; //create a client which can connect to server ip & port; WifiClient defined by client.connect()
>> 
>> void setup() {
>>   //begin connect WiFi:
>>   while (status != WL_CONNECTED) {
>>     Serial.print("attempting connection to: ");
>>     Serial.println(ssid);
>>     status = WiFi.begin(ssid, pass); //WPA/2 network
>>     delay(3000);
>>   }
>>   mlx.begin(); //start the sensors
>> }
>>
>> void loop() {
>>   WiFiClient client;
>> 
>>   float ambient = 0;
>>   float object = 0;
>>   //debug the raw sensor readings
>>   ambient = mlx.readAmbientTempF();
>>   Serial.println(ambient);
>>   object = mlx.readObjectTempF();
>>   Serial.println(object);
>>   //stringify for concatenation in jsonData 
>>   String ambientStr = String(ambient, 1);
>>   String objectStr = String(object, 1);
>>   //building the formated string
>>   String jsonData = "{\"ambientTemp\":"+ ambientStr +",\"objectTemp\":"+ objectStr +"}";
>>   Serial.println(jsonData); // debug format of json data string
>>   size_t jsonLen = jsonData.length(); //get length of json data string as unsigned int
>>   //using port 57391
>>   if (client.connect(server, 57391)) {
>>     Serial.println("conn success");
>>     //client.print is how the data get's sent, like Serial.print, but over http to your pre-defined "char server[]" 
>>     client.println("POST /Temp.php HTTP/1.1"); 
>>     client.println("Host: 192.168.1.104"); //server ip
>>     client.println("Content-Type: application/json");
>>     client.print("Content-Length: "); //no `println`, the length below must be on same line
>>     client.println(jsonLen); //length of json data being sent
>>     client.println();//empty line = end-of-header
>>     client.print(jsonData);//sends json
>>     //timeout-wait for server response
>>     unsigned long timeout = millis();
>>     while (client.connected() && millis() - timeout < 5000) {
>>       if (client.available()) {
>>         String line = client.readStringUntil('\n');
>>         Serial.println(line); //print server response
>>         timeout = millis();
>>       }
>>     }
>>     client.stop(); //close connection
>>     Serial.println("conn closed");  
>>
>>   delay(10000); //10sec is very slow, decrease to measure and send more frequently 
>>   }
>> }
>> ```
> #### [Temp.php](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/Temp.php)       
> #### [apache2.conf](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/apache2.conf)       
> #### [arduino_secrets.h](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/arduino_secrets.h)      
> #### [ports.conf](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/ports.conf)
> 
> ## \[ 2.1 \] Raspberry Pi and Arduino
> 
>> **The orginal plan called for using a Particle Photon.** 
>>   
>> **However, the Particle Cloud which receives and forwards `Particle.publish()` events will only forward to public hosts.**  
>> **Additionally, the Photon proved finicky when reading from the [ I<sup>2</sup>C ](https://i2cdevices.org/resources) : "[ Serial Bus ](https://en.wikipedia.org/wiki/I%C2%B2C)" [ MLX90614 ](https://www.amazon.com/dp/B0B63K5V7T?ref=ppx_yo2ov_dt_b_fed_asin_title).**  
>>   
>> Enter the Arduino Giga R1:
>> *image*
>> 
>> **<ins>Raspberry Pi 3b+ *and*  Pi 5</ins>:**  
>>> **Ubuntu Server 24.10 "oracular"**  
>>>> ***❗*when running sentiment analysis with Transformers, the Pi 3b+ ran out of memory and the process was killed.**
>>>> 
>>> **Python3.12.7**
>>> **python3-venv**  
>>> **joblib**  
>>> **numpy**  
>>> **scipy**  
>>> **pandas**  
>>> **transformers**  
>>> **scikit-learn**  
>>> **tensorflow**
>>> **Apache2**  
>>> **PHP**  
>>> **SQLite**  
>
