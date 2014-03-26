#include "Character.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include <memory>

struct FACEINFO
{
	unsigned int options		: 5;
	unsigned int optionsColor	: 3;

	unsigned int type			: 6;
	unsigned int jaw			: 2;

	unsigned int mouth			: 2;
	unsigned int cheek			: 2;
	unsigned int nose			: 3;
	unsigned int eye			: 3;
	unsigned int iris			: 1;
	unsigned int brows			: 3;

	unsigned int unknown		: 2;
};
static_assert(sizeof(FACEINFO) == 4, "FACEINFO must be 4 bytes");

CCharacter::CCharacter()
{

}

CCharacter::CCharacter(Framework::CStream& stream)
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
	size = Framework::Xml::GetAttributeIntValue(characterNode, "Size");
	voice = Framework::Xml::GetAttributeIntValue(characterNode, "Voice");
	skinColor = Framework::Xml::GetAttributeIntValue(characterNode, "Skin");

	hairStyle = Framework::Xml::GetAttributeIntValue(characterNode, "HairStyle");
	hairColor = Framework::Xml::GetAttributeIntValue(characterNode, "HairColor");

	eyeColor = Framework::Xml::GetAttributeIntValue(characterNode, "EyeColor");

	faceType = Framework::Xml::GetAttributeIntValue(characterNode, "FaceType");
	faceBrow = Framework::Xml::GetAttributeIntValue(characterNode, "FaceBrow");
	faceEye = Framework::Xml::GetAttributeIntValue(characterNode, "FaceEye");
	faceIris = Framework::Xml::GetAttributeIntValue(characterNode, "FaceIris");
	faceNose = Framework::Xml::GetAttributeIntValue(characterNode, "FaceNose");
	faceMouth = Framework::Xml::GetAttributeIntValue(characterNode, "FaceMouth");
	faceJaw = Framework::Xml::GetAttributeIntValue(characterNode, "FaceJaw");
	faceCheek = Framework::Xml::GetAttributeIntValue(characterNode, "FaceCheek");
	faceOption1 = Framework::Xml::GetAttributeIntValue(characterNode, "FaceOption1");
	faceOption2 = Framework::Xml::GetAttributeIntValue(characterNode, "FaceOption2");

	guardian = Framework::Xml::GetAttributeIntValue(characterNode, "Guardian");
	birthMonth = Framework::Xml::GetAttributeIntValue(characterNode, "BirthMonth");
	birthDay = Framework::Xml::GetAttributeIntValue(characterNode, "BirthDay");

	headGear = Framework::Xml::GetAttributeIntValue(characterNode, "HeadGear");
	bodyGear = Framework::Xml::GetAttributeIntValue(characterNode, "BodyGear");
	legsGear = Framework::Xml::GetAttributeIntValue(characterNode, "LegsGear");
	handsGear = Framework::Xml::GetAttributeIntValue(characterNode, "HandsGear");
	feetGear = Framework::Xml::GetAttributeIntValue(characterNode, "FeetGear");
}

std::string GetFieldStringValue(MYSQL_ROW row, const Framework::MySql::CResult::FieldIndexMap& fieldIndices, const std::string& field)
{
	auto fieldIndexIterator = fieldIndices.find(field);
	if(fieldIndexIterator == std::end(fieldIndices))
	{
		throw std::exception();
	}
	auto fieldValue = row[fieldIndexIterator->second];
	return fieldValue;
}

int GetFieldIntValue(MYSQL_ROW row, const Framework::MySql::CResult::FieldIndexMap& fieldIndices, const std::string& field)
{
	auto fieldIndexIterator = fieldIndices.find(field);
	if(fieldIndexIterator == std::end(fieldIndices))
	{
		throw std::exception();
	}
	auto fieldValue = row[fieldIndexIterator->second];
	int result = atoi(fieldValue);
	return result;
}

CCharacter::CCharacter(Framework::MySql::CResult& result)
{
	auto row = result.FetchRow();
	if(!row) return;

	auto fieldIndices = result.GetFieldIndices();

	active = true;

	id = GetFieldIntValue(row, fieldIndices, "id");

	name = GetFieldStringValue(row, fieldIndices, "name");
	tribe = GetFieldIntValue(row, fieldIndices, "tribe");
	size = GetFieldIntValue(row, fieldIndices, "size");
	voice = GetFieldIntValue(row, fieldIndices, "voice");
	skinColor = GetFieldIntValue(row, fieldIndices, "skinColor");

	hairStyle = GetFieldIntValue(row, fieldIndices, "hairStyle");
	hairColor = GetFieldIntValue(row, fieldIndices, "hairColor");

	eyeColor = GetFieldIntValue(row, fieldIndices, "eyeColor");

	faceType = GetFieldIntValue(row, fieldIndices, "faceType");
	faceBrow = GetFieldIntValue(row, fieldIndices, "faceBrow");
	faceEye = GetFieldIntValue(row, fieldIndices, "faceEye");
	faceIris = GetFieldIntValue(row, fieldIndices, "faceIris");
	faceNose = GetFieldIntValue(row, fieldIndices, "faceNose");
	faceMouth = GetFieldIntValue(row, fieldIndices, "faceMouth");
	faceJaw = GetFieldIntValue(row, fieldIndices, "faceJaw");
	faceCheek = GetFieldIntValue(row, fieldIndices, "faceCheek");
	faceOption1 = GetFieldIntValue(row, fieldIndices, "faceOption1");
	faceOption2 = GetFieldIntValue(row, fieldIndices, "faceOption2");

	guardian = GetFieldIntValue(row, fieldIndices, "guardian");
	birthMonth = GetFieldIntValue(row, fieldIndices, "birthMonth");
	birthDay = GetFieldIntValue(row, fieldIndices, "birthDay");

	weapon1 = GetFieldIntValue(row, fieldIndices, "weapon1");
	headGear = GetFieldIntValue(row, fieldIndices, "headGear");
	bodyGear = GetFieldIntValue(row, fieldIndices, "bodyGear");
	legsGear = GetFieldIntValue(row, fieldIndices, "legsGear");
	handsGear = GetFieldIntValue(row, fieldIndices, "handsGear");
	feetGear = GetFieldIntValue(row, fieldIndices, "feetGear");
	waistGear = GetFieldIntValue(row, fieldIndices, "waistGear");
	rightEarGear = GetFieldIntValue(row, fieldIndices, "rightEarGear");
	leftEarGear = GetFieldIntValue(row, fieldIndices, "leftEarGear");
	rightFingerGear = GetFieldIntValue(row, fieldIndices, "rightFingerGear");
	leftFingerGear = GetFieldIntValue(row, fieldIndices, "leftFingerGear");
}

CCharacter::~CCharacter()
{

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
	faceInfo.optionsColor = faceOption2;
	faceInfo.type = faceType;
	faceInfo.mouth = faceMouth;
	faceInfo.cheek = faceCheek;
	faceInfo.nose = faceNose;
	faceInfo.jaw = faceJaw;
	faceInfo.eye = faceEye;
	faceInfo.iris = faceIris;
	faceInfo.brows = faceBrow;
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
