#pragma once

#include <memory>
#include <vector>

class CResourceNode;
typedef std::shared_ptr<CResourceNode> ResourceNodePtr;
typedef std::vector<ResourceNodePtr> ResourceNodeArray;

class CResourceNode : public std::enable_shared_from_this<CResourceNode>
{
public:
								CResourceNode();
	virtual						~CResourceNode();

	void						AddChild(const ResourceNodePtr&);
	ResourceNodePtr				GetParent() const;
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

	template <typename NodeType>
	std::shared_ptr<NodeType> SelectNode()
	{
		auto nodes = SelectNodes<NodeType>();
		if(nodes.empty()) return std::shared_ptr<NodeType>();
		return nodes[0];
	}

protected:
	std::weak_ptr<CResourceNode>	m_parent;
	ResourceNodeArray				m_children;
};
