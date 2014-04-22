#pragma once

#include "PalleonEngine.h"
#include "UmbralMesh.h"
#include "UmbralModel.h"

class CUmbralMap : public Palleon::CMeshProvider
{
public:
							CUmbralMap(const MapLayoutPtr&);
	virtual					~CUmbralMap();

	virtual void			GetMeshes(Palleon::MeshArray&, const Palleon::CCamera*) override;

private:
	typedef std::map<uint32, Palleon::SceneNodePtr> NodeMap;
	typedef std::list<UmbralMeshPtr> MeshList;

	Palleon::SceneNodePtr	CreateUnitTreeObject(const MapLayoutPtr&, const std::shared_ptr<CMapLayout::UNIT_TREE_OBJECT_NODE>&);

	NodeMap					m_unitTreeObjectRenderables;
	MeshList				m_instances;
};

typedef std::shared_ptr<CUmbralMap> UmbralMapPtr;
