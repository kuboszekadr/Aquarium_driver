<?php

include("connect.php"); //connection function

// Process post request
$id_sensors = explode(";", $_POST["id_sensor"]);
$values = explode(";", $_POST["value"]);
$timestamps = explode(";", $_POST["timestamp"]);

$user = $_POST["user"];
$pwd = $_POST["pwd"];

// connecting into database
$conn = Connect($user, $pwd);

// Check if connection exists
if (!$conn) { echo 'Ups... Error\n'; }

$records = count($id_sensors);
echo $id_sensors[0];

$query_pattern = "insert into public.readings (id_sensor, value, timestamp) values ";
$query_pattern = $query_pattern. "($1, $2, $3)";

for ($record = 0; $record < $records; $record++)
{
	echo $id_sensors[$record];
	$query = $query_pattern;
	$result = pg_prepare($conn, "query", $query);
	$result = pg_execute($conn, "query", array(trim($id_sensors[$record]), $values[$record], $timestamps[$record]));
}

if ($result)  { echo 'OK'; } 
else { echo 'Error have occured during data inserting'; }

pg_close();  // close the connection
?>
