delimiter $$

CREATE DATABASE `ffxiv` /*!40100 DEFAULT CHARACTER SET utf8 */$$

USE `ffxiv`$$

CREATE TABLE `ffxiv_users` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `name` varchar(45) NOT NULL,
  `passhash` char(56) NOT NULL,
  `salt` char(56) NOT NULL,
  `email` varchar(256) NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `name_UNIQUE` (`name`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8$$

CREATE TABLE `ffxiv_sessions` (
  `id` char(56) NOT NULL,
  `userid` int(11) NOT NULL,
  `expiration` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `userid_UNIQUE` (`userid`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8$$

CREATE TABLE `ffxiv_characters` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `userId` int(11) NOT NULL,
  `name` varchar(21) NOT NULL,
  `tribe` smallint(6) NOT NULL,
  `size` smallint(6) NOT NULL,
  `voice` smallint(6) NOT NULL,
  `skinColor` smallint(6) NOT NULL,
  `hairStyle` smallint(6) NOT NULL,
  `hairColor` smallint(6) NOT NULL,
  `hairOption` smallint(6) NOT NULL,
  `eyeColor` smallint(6) NOT NULL,
  `faceType` smallint(6) NOT NULL,
  `faceBrow` smallint(6) NOT NULL,
  `faceEye` smallint(6) NOT NULL,
  `faceIris` smallint(6) NOT NULL,
  `faceNose` smallint(6) NOT NULL,
  `faceMouth` smallint(6) NOT NULL,
  `faceJaw` smallint(6) NOT NULL,
  `faceCheek` smallint(6) NOT NULL,
  `faceOption1` smallint(6) NOT NULL,
  `faceOption2` smallint(6) NOT NULL,
  `guardian` smallint(6) NOT NULL,
  `birthMonth` smallint(6) NOT NULL,
  `birthDay` smallint(6) NOT NULL,
  `allegiance` smallint(6) NOT NULL,
  `weapon1` int(11) NOT NULL,
  `weapon2` int(11) NOT NULL,
  `headGear` int(11) NOT NULL,
  `bodyGear` int(11) NOT NULL,
  `legsGear` int(11) NOT NULL,
  `handsGear` int(11) NOT NULL,
  `feetGear` int(11) NOT NULL,
  `waistGear` int(11) NOT NULL,
  `rightEarGear` int(11) NOT NULL,
  `leftEarGear` int(11) NOT NULL,
  `rightFingerGear` int(11) NOT NULL,
  `leftFingerGear` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8$$
