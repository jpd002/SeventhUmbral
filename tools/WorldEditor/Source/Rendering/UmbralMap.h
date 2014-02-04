#pragma once

#include "AthenaEngine.h"
#include "UmbralMesh.h"
#include "UmbralModel.h"
#include "../MapLayout.h"

class CUmbralMap : public Athena::CMeshProvider
{
public:
							CUmbralMap(const MapLayoutPtr&);
	virtual					~CUmbralMap();

	virtual void			GetMeshes(Athena::MeshArray&, const Athena::CCamera*) override;

private:
	typedef std::map<uint32, Athena::SceneNodePtr> NodeMap;
	typedef std::list<UmbralMeshPtr> MeshList;

	Athena::SceneNodePtr	CreateUnitTreeObject(const MapLayoutPtr&, const std::shared_ptr<CMapLayout::UNIT_TREE_OBJECT_NODE>&);

	NodeMap					m_unitTreeObjectRenderables;
	MeshList				m_instances;
};

typedef std::shared_ptr<CUmbralMap> UmbralMapPtr;
