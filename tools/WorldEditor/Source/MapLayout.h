#pragma once

#include <vector>
#include <map>
#include <memory>
#include "Stream.h"

class CMapLayout
{
public:
	struct RESOURCE_ITEM
	{
		char			name[0x10];
		uint32			type;
		uint32			resourceId;
		uint32			unknown1;
		uint32			unknown2;
	};
	static_assert(sizeof(RESOURCE_ITEM) == 0x20, "RESOURCE_ITEM size must be 32 bytes.");
	typedef std::vector<RESOURCE_ITEM> ResourceItemArray;

	struct LAYOUT_NODE
	{
		virtual			~LAYOUT_NODE() {}

		uint32			nodeId;
		uint32			parentPtr;
		std::string		name;
	};
	typedef std::shared_ptr<LAYOUT_NODE> LayoutNodePtr;
	typedef std::map<uint32, LayoutNodePtr> LayoutNodeMap;

	struct UNIT_TREE_OBJECT_ITEM
	{
		std::string		name;
		uint32			nodePtr;
	};
	typedef std::vector<UNIT_TREE_OBJECT_ITEM> UnitTreeObjectItemArray;

	struct UNIT_TREE_OBJECT_NODE : public LAYOUT_NODE
	{
		UnitTreeObjectItemArray		items;
	};

	struct INSTANCE_OBJECT_NODE : public LAYOUT_NODE
	{
		uint32			refNodePtr;
		float			posX;
		float			posY;
		float			posZ;
	};

	struct BGPARTS_BASE_OBJECT_NODE : public LAYOUT_NODE
	{
		std::string		modelName;
		std::string		resourceName;
		float			minX, minY, minZ;
		float			maxX, maxY, maxZ;
	};

								CMapLayout();
	virtual						~CMapLayout();

	void						Read(Framework::CStream&);

	const ResourceItemArray&	GetResourceItems() const;
	const LayoutNodeMap&		GetLayoutNodes() const;

private:
	void						LoadResourceItems();

	ResourceItemArray			m_resourceItems;
	LayoutNodeMap				m_layoutNodes;
};

typedef std::shared_ptr<CMapLayout> MapLayoutPtr;
