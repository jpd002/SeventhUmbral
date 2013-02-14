#pragma once

#include "Stream.h"
#include <string>

class CCharacter
{
public:
					CCharacter();
	virtual			~CCharacter();

	void			Load(Framework::CStream&);

	uint32			GetColorInfo() const;
	uint32			GetFaceInfo() const;

	static int		GetModelFromTribe(int);

	bool			active;

	std::string		name;

	int				tribe;
	int				voice;
	int				skinColor;

	int				hairStyle;
	int				hairColor;
	int				eyeColor;

	int				faceType;
	int				faceBrow;
	int				faceEye;
	int				faceIris;
	int				faceNose;
	int				faceMouth;
	int				faceJaw;
	int				faceCheek;
	int				faceOption1;
	int				faceOption2;

	int				headGear;
	int				bodyGear;
	int				legsGear;
	int				handsGear;
	int				feetGear;
};
