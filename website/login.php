<!DOCTYPE HTML>
<?php 

include("database.php"); 

function LoginPage_Login()
{
	$username 		= trim($_POST["username"]);
	$password 		= trim($_POST["password"]);
	
	if(empty($username))
	{
		throw new Exception("You must enter an username.");
	}
	
	if(empty($password))
	{
		throw new Exception("You must enter a password.");
	}
	
	$userId = VerifyUser($username, $password);
	return CreateSession($userId);
}

$loginError = "";
$sessionId = "";

if(isset($_POST["login"]))
{
	try
	{
		$sessionId = LoginPage_Login();
	}
	catch(Exception $e)
	{
		$loginError = $e->getMessage();
	}
}

?>
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>Seventh Umbral FFXIV Server - Login</title>
		<link rel="stylesheet" type="text/css" href="css/reset.css" />	
		<link rel="stylesheet" type="text/css" href="css/global.css" />	
	</head>
	<body>
		<?php
			if(!empty($sessionId))
			{
				echo "<script>window.location=\"ffxiv://login_success?sessionId=" . $sessionId . "\";</script>";
			}
		?>
		<form method="post" autocomplete="off">
			<table>
				<tr>
					<td>Username:</td>
					<td><input type="text" name="username" /></td>
				</tr>
				<tr>
					<td>Password:</td>
					<td><input type="password" name="password" /></td>
				</tr>
				<tr>
					<td>
						<input type="submit" name="login" value="Login" />
					</td>
				</tr>
			</table>
		</form>
		<p class="errorMessage"><?php echo($loginError); ?></p>
	</body>
</html>
