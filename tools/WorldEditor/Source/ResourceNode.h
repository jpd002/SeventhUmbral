#pragma once

#include <memory>
#include <vector>

class CResourceNode;
typedef std::shared_ptr<CResourceNode> ResourceNodePtr;
typedef std::vector<ResourceNodePtr> ResourceNodeArray;

class CResourceNode
{
public:
	virtual						~CResourceNode() {};

	const ResourceNodeArray&	GetChildren() const;

	template <typename NodeType>
	std::vector<std::shared_ptr<NodeType>> SelectNodes()
	{
		std::vector<std::shared_ptr<NodeType>> result;
		for(const auto& child : m_children)
		{
			if(auto castedChild = std::dynamic_pointer_cast<NodeType>(child))
			{
				result.push_back(castedChild);
			}
			auto childResult = child->SelectNodes<NodeType>();
			result.insert(result.end(), std::begin(childResult), std::end(childResult));
		}
		return result;
	}

protected:
	ResourceNodeArray			m_children;
};
