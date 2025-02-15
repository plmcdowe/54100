<?php

//statically declarring the IP of Giga R1 board for an ""acl""
$arduino = '192.168.1.130';

//if the http header source ip is not the Arduino's ip, exit.
//this is far from bullet-proof, header info can be spoofed, manipulated, etc. but, does add a layer of obfuscation.
if($_SERVER['REMOTE_ADDR'] != $arduino) {
        exit();
}

header('Content-Type: application/json');

try {
    //read in post contents
    $rawData = file_get_contents('php://input');

    //decode 
    $data = json_decode($rawData, true);

    //validating json data
    if (json_last_error() !== JSON_ERROR_NONE) {
        throw new Exception('invalid json: ' . json_last_error_msg());
    }

    //pull values from json keys
    $ambientTemp = isset($data['ambientTemp']) ? $data['ambientTemp'] : null;
    $objectTemp = isset($data['objectTemp']) ? $data['objectTemp'] : null;

    //ensure not null or empty
    if ($ambientTemp === null || $objectTemp === null) {
        throw new Exception('missing values');
    }

    //only accepting float values, exit otherwise
    if(!is_float($ambientTemp) || !is_float($objectTemp)) {
            exit();
    }

    //stringify for regex input validation
    $stringAmbient = (string)$ambientTemp;
    $stringObject = (string)$objectTemp;

    //MLX90614 Ambient sensing range is -40*F >> +257*F :and: Object sensing range is -94*F >> +716*F
    //but our equipment never drops below 0*F, and the arduino sends floats to the third decimal
    //performing pattern validation prevents broken or malicious values outside possibility from being inserted
    //such values would impact outlier detection at train and test time
    $ambientPt = '/(^0|[1-9]|[1-9][0-9]|1[0-9]{2}|2[1-5][1-7])\.[0-9]{1,3}$/';
    $objectPt = '/(^0|[1-9]|[1-9][0-9]|[1-6][0-9]{2}|7[0-1][1-6])\.[0-9]{1,3}$/';
    if(!preg_match($ambientPt, $stringAmbient) || !preg_match($objectPt, $stringObject)) {
            exit();
    }

    //return values to float for insert into the database
    $ambientTemp = floatval($stringAmbient);
    $objectTemp = floatval($stringObject);

    //connect to the sqlite db
    $db = new PDO('sqlite:iot_temp.db');
    $db->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    //prepare the INSERT statement
    $stmt = $db->prepare("INSERT INTO readings (ambientTemp, objectTemp) VALUES (:ambientTemp, :objectTemp)");
    $stmt->bindParam(':ambientTemp', $ambientTemp);
    $stmt->bindParam(':objectTemp', $objectTemp);

    //execute the statement
    $stmt->execute();

    //get the last inserted ID
    $lastId = $db->lastInsertId();

    //prepare response
    $response = [
        'status' => 'success',
        'message' => 'Data received and stored successfully',
        'data' => [
            'id' => $lastId,
            'ambientTemp' => $ambientTemp,
            'objectTemp' => $objectTemp,
            'timestamp' => date('Y-m-d H:i:s')
        ]
    ];

    //send the response
    echo json_encode($response);

} catch (Exception $e) {
    $errorResponse = [
        'status' => 'error',
        'message' => $e->getMessage()
    ];
    echo json_encode($errorResponse);
}
?>