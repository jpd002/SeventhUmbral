delimiter $$

CREATE DATABASE `ffxiv` /*!40100 DEFAULT CHARACTER SET utf8 */$$

USE `ffxiv`;

CREATE TABLE `ffxiv_users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `passhash` char(56) NOT NULL,
  `salt` char(56) NOT NULL,
  `email` varchar(256) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name_UNIQUE` (`name`)
) ENGINE=InnoDB AUTO_INCREMENT=7 DEFAULT CHARSET=utf8$$

CREATE TABLE `ffxiv_sessions` (
  `id` char(56) NOT NULL,
  `userid` int(11) NOT NULL,
  `expiration` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `userid_UNIQUE` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8$$

