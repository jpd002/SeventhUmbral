#pragma once

#include "Stream.h"
#include "mysql/Result.h"
#include <string>

class CCharacter
{
public:
					CCharacter();
					CCharacter(Framework::CStream&);
					CCharacter(Framework::MySql::CResult&);
	virtual			~CCharacter();

	uint32			GetColorInfo() const;
	uint32			GetFaceInfo() const;

	static int		GetModelFromTribe(int);

	bool			active = false;

	std::string		name;

	int				id = 0;

	int				tribe = 0;
	int				size = 0;
	int				voice = 0;
	int				skinColor = 0;

	int				hairStyle = 0;
	int				hairColor = 0;
	int				eyeColor = 0;

	int				faceType = 0;
	int				faceBrow = 0;
	int				faceEye = 0;
	int				faceIris = 0;
	int				faceNose = 0;
	int				faceMouth = 0;
	int				faceJaw = 0;
	int				faceCheek = 0;
	int				faceOption1 = 0;
	int				faceOption2 = 0;

	int				guardian = 0;
	int				birthMonth = 0;
	int				birthDay = 0;

	int				weapon1 = 0;

	int				headGear = 0;
	int				bodyGear = 0;
	int				legsGear = 0;
	int				handsGear = 0;
	int				feetGear = 0;
	int				waistGear = 0;
	int				rightEarGear = 0;
	int				leftEarGear = 0;
	int				rightFingerGear = 0;
	int				leftFingerGear = 0;
};
