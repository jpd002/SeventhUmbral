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
	typedef std::map<uint32, UmbralModelPtr> BgPartObjectMap;
	typedef std::map<uint32, Palleon::SceneNodePtr> UnitTreeObjectMap;

	typedef std::list<UmbralMeshPtr> MeshList;

	UmbralModelPtr			CreateBgPartObject(const MapLayoutPtr&, const std::shared_ptr<CMapLayout::BGPARTS_BASE_OBJECT_NODE>&);
	Palleon::SceneNodePtr	CreateUnitTreeObject(const MapLayoutPtr&, const std::shared_ptr<CMapLayout::UNIT_TREE_OBJECT_NODE>&);

	BgPartObjectMap			m_bgPartObjects;
	UnitTreeObjectMap		m_unitTreeObjects;

	//Stuff we render
	MeshList				m_instances;
};

typedef std::shared_ptr<CUmbralMap> UmbralMapPtr;
