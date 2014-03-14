<?php 

error_reporting(E_ALL | E_STRICT);

include("config.php");
include("database.php"); 
include("control_panel_common.php");

$g_userCharacters = GetUserCharacters($g_databaseConnection, $g_userId);

?>
<!DOCTYPE HTML>
<html xmlns="http://www.w3.org/1999/xhtml">
	<head>
		<meta charset="UTF-8">
		<title>Seventh Umbral Server</title>
		<link rel="stylesheet" type="text/css" href="css/reset.css" />	
		<link rel="stylesheet" type="text/css" href="css/global.css" />	
	</head>
	<body>
		<?php include("header.php"); ?>
		<?php include("control_panel_header.php"); ?>
		<p>
		<div class="edit">
			<h2>Characters</h2>
			<br />
			<table class="editForm">
				<tr>
					<th>Name</th>
					<th>Action</th>
				</tr>
				<?php
					foreach($g_userCharacters as $character)
					{
				?>
					<tr>
						<td><?php echo $character["name"]; ?></td>
						<td>
							<a href="control_panel_edit_character.php?id=<?php echo $character["id"]; ?>">
								Edit
							</a>
						</td>
					</tr>
				<?php
					}
				?>
			</table>
		</p>
	</body>
</html>
