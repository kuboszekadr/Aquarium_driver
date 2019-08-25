<?php
	function Connect($user, $pwd){
		$host = "localhost";
		$port = 5432;
		$db = "aquarium";
		
		$conn_str = "host=$host port=$port dbname=$db user=$user password=$pwd".PHP_EOL;
		$conn = pg_connect($conn_str);

		if (!$conn) {
			die('Connection error.');
		}
		return $conn;
	}
?>