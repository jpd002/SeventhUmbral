<?php 

include("config.php");
include("database.php");
require_once("recaptchalib.php");

function CreateUserPage_CreateUser($databaseConnection)
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
	
	InsertUser($databaseConnection, $username, $hashedPassword, $salt, $email);
}

$createUserError = "";
$enteredUserName = "";
$enteredEmail = "";

if(isset($_POST["createUser"]))
{
	$enteredUserName = $_POST["username"];
	$enteredEmail = $_POST["email"];
	try
	{
		$resp = recaptcha_check_answer(
									$recaptcha_privatekey, $_SERVER["REMOTE_ADDR"],
									$_POST["recaptcha_challenge_field"], $_POST["recaptcha_response_field"]
									);
		if(!$resp->is_valid)
		{
			throw new Exception("CAPTCHA test failed. Try again.");
		}
		CreateUserPage_CreateUser($g_databaseConnection);
		header("Location: create_user_success.php");
		die();
	}
	catch(Exception $e)
	{
		$createUserError = $e->getMessage();
	}
}

?>
<!DOCTYPE HTML>
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<title>Seventh Umbral Server</title>
		<link rel="stylesheet" type="text/css" href="css/reset.css" />	
		<link rel="stylesheet" type="text/css" href="css/global.css" />	
	</head>
	<body>
		<?php include("header.php"); ?>
		<div class="info">
			<h2>Create New User</h2>
			<br />
			<form method="post" autocomplete="off">
				<table class="infoForm">
					<tr>
						<td>Username:</td>
						<td><input type="text" name="username" value="<?php echo $enteredUserName; ?>" /></td>
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
						<td><input type="text" name="email" value="<?php echo $enteredEmail; ?>" /></td>
					</tr>
					<tr>
						<td colspan="2">
							<?php echo recaptcha_get_html($recaptcha_publickey); ?>
						</td>
					</tr>
					<tr>
						<td colspan="2">
							<input type="submit" name="createUser" value="Create User" />
						</td>
					</tr>
				</table>
			</form>
			<p class="errorMessage"><?php echo($createUserError); ?></p>
		</div>
	</body>
</html>
