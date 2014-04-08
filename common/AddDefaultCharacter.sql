delimiter $$

USE `ffxiv`$$

INSERT INTO ffxiv_characters
(
	userId, name, tribe, size, voice, skinColor, hairStyle, hairColor, hairOption, eyeColor, faceType, faceBrow, faceEye, faceIris,
	faceNose, faceMouth, faceJaw, faceCheek, faceOption1, faceOption2, guardian, birthMonth, birthDay, allegiance,
	weapon1, weapon2, headGear, bodyGear, legsGear, handsGear, feetGear, waistGear, 
	rightEarGear, leftEarGear, rightFingerGear, leftFingerGear
) 
SELECT
	id, 'Miraudont', 6, 0, 13, 66, 4, 63, 0, 59, 7, 2, 4, 0, 
	5, 3, 0, 2, 30, 3, 7, 4, 19, 1,
	0, 0, 19503, 14598, 3268, 14560, 13475, 0,
	0, 0, 0, 0
FROM
	ffxiv_users
$$
