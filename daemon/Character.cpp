#include "Character.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include <memory>

struct FACEINFO
{
	unsigned int options		: 5;
	unsigned int optionsColor	: 3;

	unsigned int eyes			: 3;
	unsigned int unknown1		: 3;
	unsigned int ears			: 2;

	unsigned int unknown2		: 4;
	unsigned int nose			: 3;
	unsigned int unknown3		: 2;
	unsigned int brows			: 3;

	unsigned int unknown4		: 4;
};
static_assert(sizeof(FACEINFO) == 4, "FACEINFO must be 4 bytes");

CCharacter::CCharacter()
: active(false)
, tribe(0)
, voice(0)
, skinColor(0)
, hairStyle(0)
, hairColor(0)
, eyeColor(0)
, faceBrow(0)
, faceEye(0)
, faceIris(0)
, faceNose(0)
, faceMouth(0)
, faceJaw(0)
, faceCheek(0)
, faceOption1(0)
, faceOption2(0)
, headGear(0)
, bodyGear(0)
, legsGear(0)
, handsGear(0)
, feetGear(0)
{

}

CCharacter::~CCharacter()
{

}

void CCharacter::Load(Framework::CStream& stream)
{
	auto rootNode = std::unique_ptr<Framework::Xml::CNode>(Framework::Xml::CParser::ParseDocument(stream));
	if(!rootNode)
	{
		return;
	}

	auto characterNode = rootNode->Select("Character");
	name = characterNode->GetAttribute("Name");
	Framework::Xml::GetAttributeBoolValue(characterNode, "Active", &active);
	tribe = Framework::Xml::GetAttributeIntValue(characterNode, "Tribe");
	voice = Framework::Xml::GetAttributeIntValue(characterNode, "Voice");
	skinColor = Framework::Xml::GetAttributeIntValue(characterNode, "Skin");

	hairStyle = Framework::Xml::GetAttributeIntValue(characterNode, "HairStyle");
	hairColor = Framework::Xml::GetAttributeIntValue(characterNode, "HairColor");

	eyeColor = Framework::Xml::GetAttributeIntValue(characterNode, "EyeColor");

	faceBrow = Framework::Xml::GetAttributeIntValue(characterNode, "FaceBrow");
	faceEye = Framework::Xml::GetAttributeIntValue(characterNode, "FaceEye");
	faceIris = Framework::Xml::GetAttributeIntValue(characterNode, "FaceIris");
	faceNose = Framework::Xml::GetAttributeIntValue(characterNode, "FaceNose");
	faceMouth = Framework::Xml::GetAttributeIntValue(characterNode, "FaceMouth");
	faceJaw = Framework::Xml::GetAttributeIntValue(characterNode, "FaceJaw");
	faceCheek = Framework::Xml::GetAttributeIntValue(characterNode, "FaceCheek");
	faceOption1 = Framework::Xml::GetAttributeIntValue(characterNode, "FaceOption1");
	faceOption2 = Framework::Xml::GetAttributeIntValue(characterNode, "FaceOption2");

	headGear = Framework::Xml::GetAttributeIntValue(characterNode, "HeadGear");
	bodyGear = Framework::Xml::GetAttributeIntValue(characterNode, "BodyGear");
	legsGear = Framework::Xml::GetAttributeIntValue(characterNode, "LegsGear");
	handsGear = Framework::Xml::GetAttributeIntValue(characterNode, "HandsGear");
	feetGear = Framework::Xml::GetAttributeIntValue(characterNode, "FeetGear");
}

uint32 CCharacter::GetColorInfo() const
{
	uint32 colorVal = skinColor | (hairColor << 10) | (eyeColor << 20);
	return colorVal;
}

uint32 CCharacter::GetFaceInfo() const
{
	FACEINFO faceInfo;
	memset(&faceInfo, 0, sizeof(FACEINFO));
	faceInfo.options = faceOption1;
	faceInfo.optionsColor = 0;
	faceInfo.eyes = 1;
	faceInfo.nose = faceNose;
	faceInfo.ears = 2;
	faceInfo.brows = faceBrow;
	faceInfo.unknown4 = 0;
	uint32 faceValue = *reinterpret_cast<uint32*>(&faceInfo);
	return faceValue;
}

int CCharacter::GetModelFromTribe(int tribe)
{
	switch(tribe)
	{
	//Hyur Midlander Male
	case 0:
	default:
		return 1;

	//Hyur Midlander Female
	case 1:
	case 2:
		return 2;

	//Elezen Male
	case 4:
	case 6:
		return 3;

	//Elezen Female
	case 5:
	case 7:
		return 4;

	//Lalafell Male
	case 8:
	case 10:
		return 5;

	//Lalafell Female
	case 9:
	case 11:
		return 6;

	//Miqo'te Female
	case 12:
	case 13:
		return 8;

	//Roegadyn Male
	case 14:
	case 15:
		return 7;

	//Hyur Highlander Male
	case 3:
		return 9;
	}
}
