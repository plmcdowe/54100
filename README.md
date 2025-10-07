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
### 2. 📂 [IOT-Arduino-Pi5](https://github.com/plmcdowe/54100/tree/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5)
   - This directory contains all files necessary to run SQLite-ML on a Raspberry Pi 5 with data from an Arduino Giga R1.
   - IOT-Arduino-Pi5 is documented after SQLite-ML-Files.

# \[ 1 \] SQLite-ML-Files:
> #### 📄 [ml_extension.c](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/ml_extension.c)    
> #### 📄 [ml_module.py](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/ml_module.py)     
> #### 📄 [shell.c](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/shell.c)     
> #### 📄 [sqlite3](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3)     
> #### 📄 [sqlite3.h](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3.h)     
> #### 📄 [sqlite3ext.h](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/SQLite-ML-Files/sqlite3ext.h)
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
>> **<ins>Windows 11 laptop running</ins>:**     
>>> **Python 3.12.3**    
>>> **Python 3-venv**    
>>> **joblib**    
>>> **numpy**    
>>> **scipy**    
>>> **pandas**    
>>> **transformers**    
>>> **scikit-learn**    
>>> **tensorflow**
> # Steps to run:  
>> ## A. Prepare Ubuntu Desktop as bootable media on a USB:  
>> [ *(click me if you would prefer Ubuntu's documentation)*: ](https://ubuntu.com/tutorials/create-a-usb-stick-on-windows#3-usb-selection)
>> ### [ *Download Ubuntu Desktop.* ](https://ubuntu.com/download/desktop/thank-you?version=24.04.1&architecture=amd64&lts=true)  
>> ### [ *Download, then launch Rufus.* ](https://github.com/pbatard/rufus/releases/download/v4.6/rufus-4.6.exe)  
>> ^ `Device` = *Select your **USB***  
>> ^ `Boot Selection` =  *"Disk or ISO image"*
>>> -- `"SELECT"` > *The Ubuntu Desktop ISO you downloaded*  
>> ^ All other options can be left default.  
>> ^ Click `Start` and acknowledge prompts. 
>>    
>> ## B. Boot & Install Ubuntu:
>> [ *(click me if you would prefer Ubuntu's documentation)*: ](https://ubuntu.com/tutorials/install-ubuntu-desktop#4-boot-from-usb-flash-drive)    
>> With both the external NVMe (for installation onto) and the bootable-ISO USB connected:  
>>> b1. Reboot the Windows device - begin pressing `F12` rapidly to ensure the machine boots into a one-time boot menu.  
>>> b2. Once in BIOS boot menu, select the bootable-ISO USB as the device to boot from.  
>>> b3. This will begin the Ubuntu installation process.  
>>> b4. At "Disk Setup" we selected "Manual Installation" and created the individual partions required for Ubuntu's install.  
>>> As Ubunut's documentation shows, you *should* be able to select an external drive from the drop down menu.
>>> 
>>> b5. Follow the Installation prompts for options such as user creation, networking, etc.  
>>  
>> ## C. Apply Updates & Install Packages:  
>>> ```Bash
>>> sudo apt update  
>>> sudo apt upgrade  
>>> sudo apt install -y build-essential  
>>> sudo apt install -y python3-pip python3.12 python3-venv python3-dev  
>>> sudo apt install -y build-essential zlib1g-dev libncurses5-dev libgdbm-dev libnss3-dev libssl-dev libreadline-dev libffi-dev libsqlite3-dev wget libbz2-dev  
>>> sudo apt install curl  
>>> sudo apt update  
>>> sudo apt install sqlite3  
>>> sudo apt install php libapache2-mod-php  
>>> sudo apt install php-cli  
>>> sudo apt install php-sqlite3  
>>> sudo apt update
>>> ```  
>>   
>> ## D. Set up your python venv:    
>>> ```Bash
>>> python3 -m venv 541v1  
>>> source 541v1/bin/activate  
>>> pip3 install --upgrade joblib  
>>> pip3 install --upgrade numpy  
>>> pip3 install --upgrade scipy  
>>> pip3 install --upgrade pandas  
>>> pip3 install --upgrade transformers  
>>> pip3 install --upgrade scikit-learn  
>>> pip3 install --upgrade tensorflow  
>>> pip3 install --upgrade tf-keras
>>> ```
>> 
>> ## **E. Download the `MAIN-3460100` directory:**    
>>> Unzip the downloaded directory to some path on your host
>> 
>> ## F. Compile and Run `ml_extension` against a database:
>>> ```Bash
>>> cd /path/to/MAIN-3460100
>>> source ~/541v1/bin/activate #activate the Python venv you created earlier. Modify the path as necessary!
>>> chmod +x sqlite3  
>>> chmod +x ml_extension.c
>>> chmod +x ml_module.py  
>>> gcc -fPIC -shared -o ml_extension.so ml_extension.c -I/usr/include/python3.12 -L/lib -lpython3.12 -ldl -lm -lpthread -Wl,-rpath,/lib
>>> 
>>> #load a databse such as one of the example databases included in the directory, or your own
>>> ./sqlite3 database
>>> #in the sqlite CLI send:
>>> .load ./ml_extension  
>>> ```
>> ### *❗*If you use your own database:
>>> You will need to create the `model_cache`, `table_train`, and `table_test` tables:
>>> ```Bash
>>> CREATE TABLE model_cache (
>>>    model_id TEXT PRIMARY KEY,
>>>    model_data BLOB,
>>>    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
>>> data_type TEXT);
>>>
>>> ALTER TABLE readings ADD COLUMN split TEXT;
>>>
>>> UPDATE readings  
>>> SET split = CASE  
>>>    WHEN ABS(RANDOM() % 100) < 80 THEN 'train'  
>>>    ELSE 'test'  
>>> END; 
>>>
>>> CREATE TABLE table_train AS  
>>> SELECT * FROM readings WHERE split = 'train';  
>>>
>>> CREATE TABLE table_test AS  
>>> SELECT * FROM database_name WHERE split = 'test';  
>>>
>>> SELECT train_model('table_train', 'column1'); 
>>> ```    
>> ### If you use the `iot_readings1.db` included in the directory, then train and start queries:
>>> ```Bash
>>> SELECT train_model('readings_train', 'objectTemp');
>>> 
>>> #the following two queries are equivalent ways to return the object temps and timestamp for values that are outliers
>>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp, 0) = TRUE;
>>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp) < 0;
>>>
>>> #conversely, the next three queries will return object temps and timestamp for values that are not outliers:
>>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp, 0) = FALSE;
>>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp, 1) = TRUE;
>>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp) > 0;
>>> ```
>

# \[ 2 \] IOT-Arduino-Pi5:
> #### 📄 [541ML.ino](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/541ML.ino)      
> **Connects the Arduino Giga R1 to Wifi**    
> **Reads ambient and object temperatures**    
> **Packs the temperatures as JSON**    
> **Connects to Pi web server and publishes JSON formatted temperature readings to PHP script on Pi**    
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
> #### 📄 [Temp.php](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/Temp.php)       
> #### 📄 [apache2.conf](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/apache2.conf)       
> #### 📄 [arduino_secrets.h](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/arduino_secrets.h)      
> #### 📄 [ports.conf](https://github.com/plmcdowe/54100/blob/d2fcb17aec2104accbf6aa3f85e82535e7ac0abe/IOT-Arduino-Pi5/ports.conf)
> 
> ## \[ 2.1 \] Raspberry Pi and Arduino
> 
>> **The orginal plan called for using a Particle Photon.** 
>>   
>> **However, the Particle Cloud which receives and forwards `Particle.publish()` events,**    
>> **will only forward to public hosts.**
>> 
>> **Additionally, the Photon proved finicky when reading from the [ I<sup>2</sup>C ](https://i2cdevices.org/resources) : "[ Serial Bus ](https://en.wikipedia.org/wiki/I%C2%B2C)" [ MLX90614 ](https://www.amazon.com/dp/B0B63K5V7T?ref=ppx_yo2ov_dt_b_fed_asin_title).**  
>>   
>> **Enter the Arduino Giga R1:**    
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
