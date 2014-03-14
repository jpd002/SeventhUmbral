<?php

$g_userInfo = GetUserInfo($g_databaseConnection, $g_userId);

?>
<p>
	Welcome, <?php echo($g_userInfo["name"]); ?>. <a href="control_panel_logout.php">(Logout)</a>
</p>
