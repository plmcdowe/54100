# IOT-SQLite-ML

Future development could include use of `<<EOF` and `<<SQL` in Bash sqripts to automate interactions with the SQLite database.   

Additionally, HTTPS/TLS with mutual authentication between the Raspberry Pi and Ardunio would be a good addition.   

We did get the Certificate Authority running on the Pi and generated certificates for it and the Arduino,   
but were unable to get the Arduino to run the required SSL libraries.

---  
## **Environment(s):**
### **-- Raspberry Pi 3b+ *and*  Pi 5:**  
> *❗*when running **sentiment analysis** with Transformers, the Pi 3b+ ran out of memory and the process was killed.   
> This was not a problem for the Pi 5, which ran everything! 
> 
> **+ Ubuntu Server 24.10 "oracular"**  
>> 
>>
>> **--- Python3.12.7**
>>> **^ python3-venv**  
>>> **^ joblib**  
>>> **^ numpy**  
>>> **^ scipy**  
>>> **^ pandas**  
>>> **^ transformers**  
>>> **^ scikit-learn**  
>>> **^ tensorflow**
>>
>> **--- Apache2**  
>>> **^ PHP**  
>>> **^ SQLite**  
>>
>
### **-- Arduino Giga R1:**
> 
> The orginal plan called for using a Particle Photon. 
>   
> However, the Particle Cloud which receives and forwards `Particle.publish()` events will only forward to **public** hosts.  
> Additionally, the Photon proved finicky when reading from the [ I<sup>2</sup>C ](https://i2cdevices.org/resources) : "[ Serial Bus ](https://en.wikipedia.org/wiki/I%C2%B2C)" MLX90614 IR sensor.  
>   
> Enter the Arduino Giga R1:   
> <img src="https://github.com/user-attachments/assets/f1b67602-d4f7-4a10-862d-e35fc5133e2f" alt="Alt Text" width="346" height="600">
> 
## In this repository:
> ### [ -- sqlite3 ](https://github.com/plmcdowe/School/blob/527a85ae13aecc003411d6c7a030105852acb30f/Purdue-CS/SQLite-ML/SQLite-ML-Files/sqlite3)  
>> **Built from source: 3460100, ready to run after a `chmod +x`**
>  
> ### [ -- ml_extension.c ](https://github.com/plmcdowe/School/blob/527a85ae13aecc003411d6c7a030105852acb30f/Purdue-CS/SQLite-ML/SQLite-ML-Files/ml_extension.c)  
>> [ see the ***main* README.md** for full description ]()  
>  
> ### [ -- ml_module.py ](https://github.com/plmcdowe/School/blob/527a85ae13aecc003411d6c7a030105852acb30f/Purdue-CS/SQLite-ML/SQLite-ML-Files/ml_module.py)   
>> [ see the ***main* README.md** for full description ]()  
>
> ### [ -- 541ML.ino ](https://github.com/plmcdowe/School/blob/527a85ae13aecc003411d6c7a030105852acb30f/Purdue-CS/SQLite-ML/IOT-Arduino-Pi5/541ML.ino)
>> Arduino C program to collect object and ambient temperatures,   
>> then send them by HTTP with onboard Wifi module formatted as JSON to the Pi PHP script.   
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
>> }
---
# Steps to run:  
## 1. Prepare Ubuntu Server as bootable media on a microUSB.
> ### **[ *Download Raspberry Pi's "imager" software.* ](https://www.raspberrypi.com/software/)**  
>> ^ `Chose Device` = *your Pi model*  
>> ^ `Choose OS` =  *"Other general-purpose OS"* > *"Ubuntu"* > *"Server" (or Desktop)*  
>> ^ `Choose Storage` =  *your connected microSD card*  
>> ^ `Next` =  **This will open a pop-up** > select `Edit Settings`  
>>> -- ***check*** & input *your hostname*  
>>> -- ***check*** & input *your **username** and **password***  
>>> -- ***check*** & input *your **SSID** and **password***  
>  
## 2. Boot/Install Ubuntu; Apply Updates & Install Packages:  
> ### ***[ 2a. ]* After you sign into the CLI, run through the following commands in sequence:**
>> ```Bash
>> sudo apt update  
>> sudo apt upgrade  
>> sudo apt install -y build-essential
>> sudo apt install -y python3-pip python3.12 python3-venv python3-dev
>> sudo apt install -y build-essential zlib1g-dev libncurses5-dev libgdbm-dev libnss3-dev libssl-dev libreadline-dev libffi-dev libsqlite3-dev wget libbz2-dev  
>> sudo apt install curl  
>> sudo apt update  
>> sudo apt install sqlite3
>> sudo apt install php libapache2-mod-php
>> sudo apt install php-cli
>> sudo apt install php-sqlite3
>> sudo apt update
> ### ***[ 2b. ]* Configure Apache2 and PHP:**
>> ```Bash
>> sudo systemctl restart apache2.service
>> sudo nano /etc/apache2/apache2.conf
>> ServerName <your-server-ip>
>> Ctrl-O #to write-out your change to apache2.conf
>> /return  #to select and save as apache2.conf
>> Ctrl-X #to exit nano
>> sudo service apache2 reload #use this after config changes to apache system files
>> sudo apache2ctl configtest #to verify apache config
>> 
>> #+-----------------------------------------------------------------------------------------------------------------------+
>> # Your Pi is now a web-server and accessible at its IP from a browser on the same LAN.                                   |
>> # Let's apply some really basic security measures. But, first - a quick layout of the directory, and pertinent files.    |
>> #                                                                                                                        |
>> # system root directory, just send `cd /` to get here from anywhere.                                                     |
>> # ...................................................................                                                    |
>> # /"root"                                                                                                                |
>> #  |-- /etc/apache2/                                                                                                     |
>> #  |    |   |-- apache2.conf                                                                                             |
>> #  |    |   |-- ports.conf                                                                                               |
>> #  |    |   |-- etc/apache2/conf-enabled/                                                                                |
>> #  |    |                   |-- security.conf                                                                            |
>> #  |    |-- /etc/php/8.3/apache2                                                                                         |
>> #  |                     |-- conf.d                                                                                      |
>> #  |                     |-- php.ini                                                                                     |
>> #  |-- /var/www/                                                                                                         |
>> #       |-- index.html                                                                                                   |
>> #       |-- ``location for .php``                                                                                        |
>> #       |-- ``also default location for php_errors.log``                                                                 |
>> #+-----------------------------------------------------------------------------------------------------------------------+
>>
>> #replace 57391 with your non-standard port number; command replaces 'Listen 80'
>> sudo sed -i 's/^Listen.*/Listen 57391/g' /etc/apache2/ports.conf
>>   
>> #ensure that "expose_php" is unconmmented (no leading ';') and '= Off'
>> cat /etc/php/8.3/apache2/php.ini | grep 'expose_php ='
>>   
>> #ensure that "display_errors" and "display_startup_errors" are unconmmented (no leading ';') and '= Off'
>> cat /etc/php/8.3/apache2/php.ini | grep 'display.*_errors ='
>>   
>> #ensure that "log_errors" is unconmmented (no leading ';') and '= On'
>> cat /etc/php/8.3/apache2/php.ini | grep 'log_errors ='
>>   
>> #ensure that "error_log" is unconmmented (no leading ';') and '= php_errors.log' (or some other file of your choosing)  
>> cat /etc/php/8.3/apache2/php.ini | grep 'error_log = p'
>> #if commented out (like mine was) you can send:
>> sudo sed -i 's/^;error_log = php_errors.log/error_log = php_errors.log/g' /etc/php/8.3/apache2/php.ini
>>     
>> #ensure that "ServerTokens Prod" is uncommented, while "ServerTokens Minimial" and "Full" are commented out with '#'
>> cat etc/apache2/conf-available/security.conf | grep Ser.*Tokens.*[MPF]
>>   
>> #ensure that "ServerSignature Off" is uncommented, while "ServerSignature On" '#'
>> cat etc/apache2/conf-available/security.conf | grep Server.*Sig
>>   
>> sudo service apache2 reload
> ### ***[ 2c. ]* Add PHP the Script:**
>>```PHP
>> <?php
>> //statically declarring the IP of Giga R1 board for an ""acl""
>> $arduino = '192.168.1.130';
>> //if the http header source IP is not the Arduino's IP, exit. This is far from bullet-proof.
>> //Header info can be crafted, spoofed, manipulated; etc. but, it does add a layer of obfuscation.
>> if($_SERVER['REMOTE_ADDR'] != $arduino) {
>>         exit();
>> }
>> header('Content-Type: application/json');
>> try {
>>     //read in POST contents
>>     $rawData = file_get_contents('php://input');
>>     //decode 
>>     $data = json_decode($rawData, true);
>>     //json data error check
>>     if (json_last_error() !== JSON_ERROR_NONE) {
>>         throw new Exception('invalid json: ' . json_last_error_msg());
>>     }
>>     //pull values from json keys
>>     $ambientTemp = isset($data['ambientTemp']) ? $data['ambientTemp'] : null;
>>     $objectTemp = isset($data['objectTemp']) ? $data['objectTemp'] : null;
>>     //ensure not null or empty
>>     if ($ambientTemp === null || $objectTemp === null) {
>>         throw new Exception('missing values');
>>     }
>>     //only accepting float values, exit otherwise
>>     if(!is_float($ambientTemp) || !is_float($objectTemp)) {
>>             exit();
>>     }
>>     //stringify for regex input validation
>>     $stringAmbient = (string)$ambientTemp;
>>     $stringObject = (string)$objectTemp;
>>     //MLX90614 Ambient sensing range is -40*F >> +257*F :and: Object sensing range is -94*F >> +716*F
>>     //but our equipment never drops below 0*F, and the arduino sends floats to the third decimal
>>     //performing pattern validation prevents broken or malicious values outside possibility from being inserted
>>     //such values would impact outlier detection at train and test time
>>     $ambientPt = '/(^0|[1-9]|[1-9][0-9]|1[0-9]{2}|2[1-5][1-7])\.[0-9]{1,3}$/';
>>     $objectPt = '/(^0|[1-9]|[1-9][0-9]|[1-6][0-9]{2}|7[0-1][1-6])\.[0-9]{1,3}$/';
>>     if(!preg_match($ambientPt, $stringAmbient) || !preg_match($objectPt, $stringObject)) {
>>             exit();
>>     }
>>     //return values to float for insert into the database
>>     $ambientTemp = floatval($stringAmbient);
>>     $objectTemp = floatval($stringObject);
>>     //connect to the sqlite db
>>     $db = new PDO('sqlite:iot_temp.db');
>>     $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
>>     //prepare the INSERT statement
>>     $stmt = $db->prepare("INSERT INTO readings (ambientTemp, objectTemp) VALUES (:ambientTemp, :objectTemp)");
>>     $stmt->bindParam(':ambientTemp', $ambientTemp);
>>     $stmt->bindParam(':objectTemp', $objectTemp);
>>     //execute the statement
>>     $stmt->execute();
>>     //get the last inserted ID
>>     $lastId = $db->lastInsertId();
>>     //prepare response
>>     $response = [
>>         'status' => 'success',
>>         'message' => 'Data received and stored successfully',
>>         'data' => [
>>             'id' => $lastId,
>>             'ambientTemp' => $ambientTemp,
>>             'objectTemp' => $objectTemp,
>>             'timestamp' => date('Y-m-d H:i:s')
>>         ]
>>     ];
>>     //send the response
>>     echo json_encode($response);
>> } catch (Exception $e) {
>>     $errorResponse = [
>>         'status' => 'error',
>>         'message' => $e->getMessage()
>>     ];
>>     echo json_encode($errorResponse);
>> }
>> ?>
>> ```  
>> **NOTE:** With `$db = new PDO('sqlite:iot_temp.db');` we are just writing to a database directly in */var/www/html*  
>
> ### ***[ 2d. ]* Set permissions for the database to allow the PHP script *(www-data)* to open and write:**  
>> ```Bash
>> #create a new shared group  
>> sudo groupadd DBGRP  
>> sudo usermod -aG DBGRP <your_username>  
>> sudo usermod -aG DBGRP www-data  
>> sudo chown www-data:DBGRP /var/www/html/iot_temp.db  
>> sudo chmod 660 /var/www/html/iot_temp.db  
>> sudo chgrp DBGRP /var/www/html  
>> sudo chgrp DBGRP /var/www  
>> sudo chmod 770 /var/www  
>> sudo chmod 770 /var/www/html
>> ```
>>    
## **3. Set up your python venv:**
> ```Bash
> python3 -m venv 541v1  
> source 541v1/bin/activate  
> pip3 install --upgrade joblib  
> pip3 install --upgrade numpy  
> pip3 install --upgrade scipy  
> pip3 install --upgrade pandas  
> pip3 install --upgrade transformers  
> pip3 install --upgrade scikit-learn  
> pip3 install --upgrade tensorflow  
> pip3 install --upgrade tf-keras
> ```
## **4. Download the `IOT-3460100` directory:**
> A simple, way to download only the `IOT-3460100` directory is:  
>> Copy this permalink to the directory:    
>>>  
>>>
> Unzip the downloaded directory to some path on your host  
> 
## **5. Compile and Run `ml_extension` & `ml_module`:**  
> ```Bash
> cd /path/to/IOT-3460100
> chmod +x sqlite3  
> chmod +x ml_extension.c  
> chmod +x ml_module.py  
> gcc -fPIC -shared -o ml_extension.so ml_extension.c -I/usr/include/python3.12 -L/usr/lib -lpython3.12 -ldl -lm -lpthread -Wl,-rpath,/usr/lib  
>
> #open the iot database:
> #--if opening the database being written by the PHP script:
> ./sqlite3 /var/www/html/iot_temp.db
> 
> #--if opening the database from the IOT-3460100 directory:
> ./sqlite3 iot_temp1.db
>
> ```
> ### If you use your own database being written by the PHP script:
>> You will need to create the `model_cache`, `readings_train`, and `readings_test` tables:
>> ```Bash
>> CREATE TABLE model_cache (
>>    model_id TEXT PRIMARY KEY,
>>    model_data BLOB,
>>    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
>> data_type TEXT);
>>
>> ALTER TABLE readings ADD COLUMN split TEXT;
>>
>> UPDATE readings  
>> SET split = CASE  
>>    WHEN ABS(RANDOM() % 100) < 80 THEN 'train'  
>>    ELSE 'test'  
>> END; 
>>
>> CREATE TABLE readings_train AS  
>> SELECT * FROM readings WHERE split = 'train';  
>>
>> CREATE TABLE readings_test AS  
>> SELECT * FROM readings WHERE split = 'test';  
>>
>> SELECT train_model('readings_train', 'objectTemp'); 
>> ```
> ### If you use the `iot_readings1.db` included in the directory, then train and start queries:
>> ```Bash
>> SELECT train_model('readings_train', 'objectTemp');
>> 
>> #the following two queries are equivalent ways to return the object temps and timestamp for values that are outliers
>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp, 0) = TRUE;
>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp) < 0;
>>
>> #conversely, the next three queries will return object temps and timestamp for values that are not outliers:
>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp, 0) = FALSE;
>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp, 1) = TRUE;
>> SELECT objectTemp, timestamp FROM readings_test WHERE outlier(objectTemp) > 0;
>> ```
