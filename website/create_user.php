<!DOCTYPE HTML>
<?php 

include("database.php"); 

function CreateUserPage_CreateUser()
{
	$username 		= trim($_POST["username"]);
	$password 		= trim($_POST["password"]);
	$repeatPassword	= trim($_POST["repeatPassword"]);
	$email			= trim($_POST["email"]);
	
	if(empty($username))
	{
		throw new Exception("You must enter an username.");
	}
	
	if(empty($password))
	{
		throw new Exception("You must enter a password.");
	}
	
	if($password !== $repeatPassword)
	{
		throw new Exception("Repeated password doesn't match with entered password.");
	}
	
	if(empty($email) || !filter_var($email, FILTER_VALIDATE_EMAIL))
	{
		throw new Exception("You must enter a valid e-mail address.");
	}
	
	$salt = GenerateRandomSha224();
	$saltedPassword = $password . $salt;
	$hashedPassword = hash("sha224", $saltedPassword);
	
	InsertUser($username, $hashedPassword, $salt, $email);
}

$createUserError = "";

if(isset($_POST["createUser"]))
{
	try
	{
		CreateUserPage_CreateUser();
	}
	catch(Exception $e)
	{
		$createUserError = $e->getMessage();
	}
}

?>
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>Seventh Umbral FFXIV Server - Create User</title>
		<link rel="stylesheet" type="text/css" href="css/reset.css" />	
		<link rel="stylesheet" type="text/css" href="css/global.css" />	
	</head>
	<body>
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
					<td>Repeat Password:</td>
					<td><input type="password" name="repeatPassword" /></td>
				</tr>
				<tr>
					<td>E-mail Address:</td>
					<td><input type="text" name="email" /></td>
				</tr>
				<tr>
					<td>
						<input type="submit" name="createUser" value="Create User" />
					</td>
				</tr>
			</table>
		</form>
		<p class="errorMessage"><?php echo($createUserError); ?></p>
	</body>
</html>
