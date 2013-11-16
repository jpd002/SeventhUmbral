#include "ResourceNode.h"
#include <assert.h>

CResourceNode::CResourceNode()
{

}

CResourceNode::~CResourceNode()
{

}

ResourceNodePtr CResourceNode::GetParent() const
{
	return m_parent.lock();
}

void CResourceNode::AddChild(const ResourceNodePtr& resourceNode)
{
	assert(resourceNode->m_parent.expired());
	auto sharedPtr = shared_from_this();
	resourceNode->m_parent = sharedPtr;
	m_children.push_back(resourceNode);
}
