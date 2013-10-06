<?php
include("config.php");

$db_connect = mysql_connect($db_server, $db_username, $db_password);

if(!$db_connect)
{
	die("Error while connecting to the database");
}

mysql_select_db($db_database);
mysql_query("SET NAMES 'utf8'");

function GenerateRandomSha224()
{
	mt_srand(microtime(true) * 100000 + memory_get_usage(true));
	return hash("sha224", uniqid(mt_rand(), true));
}

function VerifyUser($username, $password)
{
	$username = mysql_real_escape_string($username);
	
	$query = sprintf("SELECT id, passhash, salt FROM ffxiv_users WHERE name = '%s'", $username);
	$queryResult = mysql_query($query);
	if(!$queryResult)
	{
		throw new Exception("Could not verify user.");
	}

	$queryRow = mysql_fetch_assoc($queryResult);
	
	$id				= $queryRow["id"];
	$storedPasshash	= $queryRow["passhash"];
	$salt 			= $queryRow["salt"];

	$saltedPassword = $password . $salt;
	$hashedPassword = hash("sha224", $saltedPassword);
	
	if($hashedPassword !== $storedPasshash)
	{
		throw new Exception("Could not verify user.");
	}
	
	return $id;
}

function InsertUser($username, $passhash, $salt, $email)
{
	$username = mysql_real_escape_string($username);
	$email = mysql_real_escape_string($email);
	
	$query = sprintf("INSERT INTO ffxiv_users (name, passhash, salt, email) VALUES ('%s', '%s', '%s', '%s')",
		$username, $passhash, $salt, $email);
	if(!mysql_query($query))
	{
		throw new Exception("Could not add user.");
	}
}

function CreateSession($userId)
{
	//Delete any session that might be active
	$query = sprintf("DELETE FROM ffxiv_sessions WHERE userId = '%d'", $userId);
	if(!mysql_query($query))
	{
		throw new Exception("Failed to create session.");
	}
	
	$sessionId = GenerateRandomSha224();
	$query = sprintf("INSERT INTO ffxiv_sessions (id, userid, expiration) VALUES ('%s', %d, NOW() + INTERVAL 15 MINUTE)", $sessionId, $userId);
	if(!mysql_query($query))
	{
		throw new Exception("Failed to create session.");
	}
	
	return $sessionId;
}

?>
