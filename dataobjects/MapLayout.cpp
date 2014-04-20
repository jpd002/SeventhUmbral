#include <vector>
#include <algorithm>
#include <map>
#include "MapLayout.h"

//#define _TRACE_TREE

#ifdef _TRACE_TREE
#include <Windows.h>
#include "string_format.h"
#endif

struct UNK1
{
	uint32 offset1;
	uint32 offset2;
	uint32 someFlags1;
	uint32 someFlags2;
};

CMapLayout::CMapLayout()
{

}

CMapLayout::~CMapLayout()
{

}

const CMapLayout::ResourceItemArray& CMapLayout::GetResourceItems() const
{
	return m_resourceItems;
}

const CMapLayout::LayoutNodeMap& CMapLayout::GetLayoutNodes() const
{
	return m_layoutNodes;
}

void CMapLayout::Read(Framework::CStream& inputStream)
{
	uint8 fileId[0x20];
	inputStream.Read(fileId, 0x20);
	uint32 headerSize = inputStream.Read32();
	uint32 resourceItemCount = inputStream.Read32();

	//Read resource items
	inputStream.Seek(0x40, Framework::STREAM_SEEK_SET);
	m_resourceItems.resize(resourceItemCount);
	for(auto& resourceItem : m_resourceItems)
	{
		inputStream.Read(&resourceItem, sizeof(RESOURCE_ITEM));
	}

	//Read scene tree
	inputStream.Seek(headerSize, Framework::STREAM_SEEK_SET);

	//Skip SEDB header
	inputStream.Seek(0x30, Framework::STREAM_SEEK_CUR);

	uint32 lybMagic = inputStream.Read32();
	uint32 lybSize = inputStream.Read32();
	uint16 unk1Count = inputStream.Read16();
	uint16 nodeCount = inputStream.Read16();

	//Skip some other headers
	inputStream.Seek(0x0C, Framework::STREAM_SEEK_CUR);

	std::vector<uint32> nodePtrs;
	nodePtrs.resize(nodeCount);
	inputStream.Read(nodePtrs.data(), nodeCount * sizeof(uint32));

	std::vector<UNK1> unk1s;
	unk1s.resize(unk1Count);
	inputStream.Read(unk1s.data(), unk1Count * sizeof(UNK1));

	std::map<uint32, std::string> nodeNames;

	for(unsigned int i = 1; i < nodeCount; i++)
	{
		uint32 nodePtr = nodePtrs[i];

		inputStream.Seek(nodePtr + headerSize + 0x30, Framework::STREAM_SEEK_SET);
		uint32 nodeHeader[3];
		inputStream.Read(nodeHeader, sizeof(nodeHeader));

		inputStream.Seek(nodeHeader[2] + headerSize + 0x30, Framework::STREAM_SEEK_SET);

		auto nodeName = inputStream.ReadString();
		nodeNames[nodePtr] = nodeName;
	}

	for(unsigned int i = 1; i < nodeCount; i++)
	{
		uint32 nodePtr = nodePtrs[i];
		uint32 nodeAbsPtr = nodePtr + headerSize + 0x30;

		inputStream.Seek(nodeAbsPtr, Framework::STREAM_SEEK_SET);
		uint32 nodeHeader[3];
		inputStream.Read(nodeHeader, sizeof(nodeHeader));

		uint32 nodeId		= nodeHeader[0];
		uint32 parentPtr	= nodeHeader[1];

		auto nodeName		= nodeNames[nodePtr];
		auto parentNodeName = nodeNames[nodeHeader[1]];

#ifdef _TRACE_TREE
		OutputDebugStringA(string_format("Id: 0x%0.8X, Ptr: 0x%0.8X(0x%0.8X), Name: %s, Parent Name: %s\r\n", 
			nodeHeader[0], nodePtr, nodeAbsPtr, nodeName.c_str(), parentNodeName.c_str()).c_str());
#endif

		LayoutNodePtr result;

		if(parentNodeName == "RefObjects/InstanceObject")
		{
			auto instanceObjectNode = std::make_shared<INSTANCE_OBJECT_NODE>();

			uint32 nodeData[0x10];

			//0x08 -> Pos X?
			//0x09 -> Pos Y?
			//0x0A -> Pos Z?
			//0x0F -> Ref Node Ptr?

			inputStream.Seek(nodeAbsPtr, Framework::STREAM_SEEK_SET);
			inputStream.Read(nodeData, sizeof(nodeData));

			instanceObjectNode->posX = *reinterpret_cast<float*>(&nodeData[0x08]);
			instanceObjectNode->posY = *reinterpret_cast<float*>(&nodeData[0x09]);
			instanceObjectNode->posZ = *reinterpret_cast<float*>(&nodeData[0x0A]);

			instanceObjectNode->refNodePtr = nodeData[0x0F];

			uint32 rotAbsPtr = nodeData[0x0B] + headerSize + 0x30;
			uint32 scaleAbsPtr = nodeData[0x0C] + headerSize + 0x30;

			float rotData[4];

			inputStream.Seek(rotAbsPtr, Framework::STREAM_SEEK_SET);
			inputStream.Read(rotData, sizeof(rotData));

			instanceObjectNode->rotX = rotData[0];
			instanceObjectNode->rotY = rotData[1];
			instanceObjectNode->rotZ = rotData[2];

			result = instanceObjectNode;
		}
		else if(parentNodeName == "RefObjects/UnitTree/UnitTreeObject")
		{
			auto unitTreeObjectNode = std::make_shared<UNIT_TREE_OBJECT_NODE>();

			uint32 nodeData[0x14];

			//0x07 -> Position Array Ptr?
			//0x08 -> Scale Array Ptr?
			//0x0B -> String Ptr
			//0x0C -> Item Array Ptr
			//0x0D -> Item Count

			inputStream.Seek(nodeAbsPtr, Framework::STREAM_SEEK_SET);
			inputStream.Read(nodeData, sizeof(nodeData));

			inputStream.Seek(nodeData[0x0C] + headerSize + 0x30, Framework::STREAM_SEEK_SET);
			unitTreeObjectNode->items.resize(nodeData[0x0D]);

			for(auto& item : unitTreeObjectNode->items)
			{
				uint32 itemValues[0x0C];
				inputStream.Read(itemValues, sizeof(itemValues));

				auto prevPtr = inputStream.Tell();

				inputStream.Seek(itemValues[0x05] + headerSize + 0x30, Framework::STREAM_SEEK_SET);
				item.name = inputStream.ReadString();

				inputStream.Seek(prevPtr, Framework::STREAM_SEEK_SET);

				item.nodePtr = itemValues[0x06];
			}

			result = unitTreeObjectNode;
		}
		else if(parentNodeName == "BaseObjects/BG/BGPartsBaseObject" || parentNodeName == "BaseObjects/BG/BGChipBaseObject")
		{
			auto bgPartsBaseObjectNode = std::make_shared<BGPARTS_BASE_OBJECT_NODE>();

			uint8 nodeData[0x54];

			inputStream.Seek(nodeAbsPtr, Framework::STREAM_SEEK_SET);
			inputStream.Read(nodeData, sizeof(nodeData));

			uint32 someStringPtr1 = *reinterpret_cast<uint32*>(nodeData + 0x08);
			uint32 someStringPtr2 = *reinterpret_cast<uint32*>(nodeData + 0x20);

			uint32 someStringAbsPtr1 = someStringPtr1 + headerSize + 0x30;
			uint32 someStringAbsPtr2 = someStringPtr2 + headerSize + 0x30;

			float vec0x = *reinterpret_cast<float*>(nodeData + 0x3C);
			float vec0y = *reinterpret_cast<float*>(nodeData + 0x40);
			float vec0z = *reinterpret_cast<float*>(nodeData + 0x44);

			float vec1x = *reinterpret_cast<float*>(nodeData + 0x48);
			float vec1y = *reinterpret_cast<float*>(nodeData + 0x4C);
			float vec1z = *reinterpret_cast<float*>(nodeData + 0x50);

			inputStream.Seek(someStringAbsPtr1, Framework::STREAM_SEEK_SET);
			auto modelName = inputStream.ReadString();

			inputStream.Seek(someStringAbsPtr2, Framework::STREAM_SEEK_SET);
			auto resourceName = inputStream.ReadString();

			bgPartsBaseObjectNode->modelName = modelName;
			bgPartsBaseObjectNode->resourceName = resourceName;
			bgPartsBaseObjectNode->minX = vec0x;
			bgPartsBaseObjectNode->minY = vec0y;
			bgPartsBaseObjectNode->minZ = vec0z;
			bgPartsBaseObjectNode->maxX = vec1x;
			bgPartsBaseObjectNode->maxY = vec1y;
			bgPartsBaseObjectNode->maxZ = vec1z;

			result = bgPartsBaseObjectNode;
		}
		else
		{
			result = std::make_shared<LAYOUT_NODE>();
		}

		result->name		= nodeName;
		result->parentPtr	= parentPtr;
		result->nodeId		= nodeId;

		m_layoutNodes.insert(std::make_pair(nodePtr, result));
	}
}
