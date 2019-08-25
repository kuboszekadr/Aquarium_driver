<?php

include("connect.php"); //connection function

// Process post request
$temp = $_POST["temp"];
$ph = $_POST["ph"];
$wls = $_POST["water_level_sump"];
$date = $_POST["date"];

$user = $_POST["user"];
$pwd = $_POST["pwd"];


// connecting into database
$conn = Connect($user, $pwd);

// Check if connection exists
if (!$conn){
echo 'Ups... Error\n';
} else {
echo 'Connection established\n';
}

$query = "insert into public.arduino_readings (ph, temp ,water_level_sump, date) values ";
$query = $query . "($1, $2, $3, $4);";

echo $query;

$result = pg_prepare($conn, "query", $query);
$result = pg_execute($conn, "query", array($ph, $temp, $wls, $date));

if ($result){
echo 'Data sent';
} else {
echo 'Error have occured during data inserting';
}

pg_close();  // close the connection
?>