<?php

if(!isset($_COOKIE["sessionId"]))
{
	header("Location: control_panel_login.php");
	exit;
}
$g_sessionId = $_COOKIE["sessionId"];

try
{
	$g_userId = GetUserIdFromSession($g_databaseConnection, $g_sessionId);
}
catch(Exception $e)
{
	header("Location: control_panel_login.php");
	exit;
}

?>
