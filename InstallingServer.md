# Installing the Seventh Umbral Server on Windows #

1. Install Apache, PHP (with extensions enabled) and MySQL. No specific versions are needed, but I personally use either Apache 2.2 or 2.4, PHP 5.5 and MySQL 5.6.

* Run the "common/CreateDatabase.sql" script on a fresh database on MySQL.

* Host the contents of the "website" folder on Apache.

* In the "website" folder, rename the "config_template.php" file to "config.php" and fill in the login information for the database. Recaptcha keys are also needed by the user creation page.

* At that point, you should be able create users and simulate logins using the website.

* Build the server using Visual Studio 2013.

* Create a "Seventh Umbral Server Data Files" folder in your "My Documents" folder. This folder will be created automatically if you run the server once even if it doesn't have proper configuration.

* Copy the "daemon/config_template.xml" file to that "Data Files" folder you've just created. Open the file and fill in the database login information as well as the IP of the game server (can be 127.0.0.1 if running locally).

* Copy the contents of the "data" folder from the repository to the same "Data Files" folder.

* Add your server in the launcher's server list. The launcher's server list file is located in the launcher's installation directory as "servers.xml".

* Start the launcher and select your server or enter its address in the combo box. The login page should show up and will allow you to start the game.
