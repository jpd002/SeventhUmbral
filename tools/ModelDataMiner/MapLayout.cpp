#include "MapLayout.h"
#include <vector>
#include <algorithm>
#include <map>

struct UNK1
{
	uint32 offset1;
	uint32 offset2;
	uint32 someFlags1;
	uint32 someFlags2;
};

CMapLayout::CMapLayout(Framework::CStream& inputStream)
{
	Load(inputStream);
}

CMapLayout::~CMapLayout()
{

}

std::string ReadString(Framework::CStream& inputStream)
{
	std::string result;
	while(1)
	{
		char value = inputStream.Read8();
		if(value == 0) break;
		result += value;
	}
	return result;
}

void CMapLayout::Load(Framework::CStream& inputStream)
{
	uint8 fileId[0x20];
	inputStream.Read(fileId, 0x20);
	uint32 headerSize = inputStream.Read32();

	inputStream.Seek(headerSize, Framework::STREAM_SEEK_SET);

	//Skip SEDB header
	inputStream.Seek(0x30, Framework::STREAM_SEEK_CUR);

	uint32 lybMagic = inputStream.Read32();
	uint32 lybSize = inputStream.Read32();
	uint16 unk1Count = inputStream.Read16();
	uint16 unk2Count = inputStream.Read16();

	//Skip some other headers
	inputStream.Seek(0x0C, Framework::STREAM_SEEK_CUR);

	std::vector<uint32> unk2s;
	unk2s.resize(unk2Count);
	inputStream.Read(unk2s.data(), unk2Count * sizeof(uint32));

	std::vector<UNK1> unk1s;
	unk1s.resize(unk1Count);
	inputStream.Read(unk1s.data(), unk1Count * sizeof(UNK1));

	std::map<uint32, std::string> nodeNames;

	for(unsigned int i = 1; i < unk2Count; i++)
	{
		uint32 nodePtr = unk2s[i];

		inputStream.Seek(nodePtr + headerSize + 0x30, Framework::STREAM_SEEK_SET);
		uint32 nodeHeader[3];
		inputStream.Read(nodeHeader, sizeof(nodeHeader));

		inputStream.Seek(nodeHeader[2] + headerSize + 0x30, Framework::STREAM_SEEK_SET);

		auto nodeName = ReadString(inputStream);
		nodeNames[nodePtr] = nodeName;
	}

	for(unsigned int i = 1; i < unk2Count; i++)
	{
		uint32 nodePtr = unk2s[i];
		uint32 nodeAbsPtr = nodePtr + headerSize + 0x30;

		inputStream.Seek(nodeAbsPtr, Framework::STREAM_SEEK_SET);
		uint32 nodeHeader[3];
		inputStream.Read(nodeHeader, sizeof(nodeHeader));

		auto nodeName = nodeNames[nodePtr];
		auto parentNodeName = nodeNames[nodeHeader[1]];

		printf("Id: 0x%0.8X, Ptr: 0x%0.8X, Name: %s, Parent Name: %s\r\n", nodeHeader[0], nodePtr, nodeName.c_str(), parentNodeName.c_str());

		if(parentNodeName == "BaseObjects/BG/BGPartsBaseObject")
		{
			uint8 nodeData[0x54];

			inputStream.Seek(nodeAbsPtr, Framework::STREAM_SEEK_SET);
			inputStream.Read(nodeData, sizeof(nodeData));

			float vec0x = *reinterpret_cast<float*>(nodeData + 0x3C);
			float vec0y = *reinterpret_cast<float*>(nodeData + 0x40);
			float vec0z = *reinterpret_cast<float*>(nodeData + 0x44);

			float vec1x = *reinterpret_cast<float*>(nodeData + 0x48);
			float vec1y = *reinterpret_cast<float*>(nodeData + 0x4C);
			float vec1z = *reinterpret_cast<float*>(nodeData + 0x50);

			printf("Bounds = (%f, %f, %f), (%f, %f, %f)\r\n", 
				vec0x, vec0y, vec0z, vec1x, vec1y, vec1z);
		}

		if(parentNodeName == "RefObjects/InstanceObject")
		{
			uint8 nodeData[0x50];

			inputStream.Seek(nodeAbsPtr, Framework::STREAM_SEEK_SET);
			inputStream.Read(nodeData, sizeof(nodeData));

			float vec0x = *reinterpret_cast<float*>(nodeData + 0x20);
			float vec0y = *reinterpret_cast<float*>(nodeData + 0x24);
			float vec0z = *reinterpret_cast<float*>(nodeData + 0x28);

			uint32 refPtr = *reinterpret_cast<uint32*>(nodeData + 0x3C);
			auto refName = nodeNames[refPtr];

			printf("Ref Name: %s, Pos = (%f, %f, %f)\r\n", refName.c_str(), vec0x, vec0y, vec0z);
		}

		if(parentNodeName == "RefObjects/UnitTree/UnitTreeObject")
		{
			uint8 nodeData[0x50];

			inputStream.Seek(nodeAbsPtr, Framework::STREAM_SEEK_SET);
			inputStream.Read(nodeData, sizeof(nodeData));

			uint32 somePtr1 = *reinterpret_cast<uint32*>(nodeData + 0x1C);
			uint32 somePtr2 = *reinterpret_cast<uint32*>(nodeData + 0x20);

			uint32 somePtr3 = *reinterpret_cast<uint32*>(nodeData + 0x30);
			uint32 itemCount = *reinterpret_cast<uint32*>(nodeData + 0x34);

			printf("Some Ptr1 = 0x%0.8X, Some Ptr2 = 0x%0.8X, Some Ptr3 = 0x%0.8X, Some Count = %d\r\n", 
				somePtr1, somePtr2, somePtr3, itemCount);
		}
	}
}
