#pragma once

#include <thread>
#include <mutex>
#include "PalleonEngine.h"
#include "UmbralMesh.h"
#include "UmbralModel.h"

class CUmbralMap : public Palleon::CMeshProvider
{
public:
							CUmbralMap(const MapLayoutPtr&);
	virtual					~CUmbralMap();

	virtual void			GetMeshes(Palleon::MeshArray&, const Palleon::CCamera*) override;

	void					CancelLoading();

private:
	struct INSTANCE_INFO
	{
		CVector3 position = CVector3(0, 0, 0);
		CQuaternion rotation;
	};

	typedef std::multimap<uint32, INSTANCE_INFO> InstanceInfoMap;
	typedef std::map<uint32, UmbralModelPtr> BgPartObjectMap;
	typedef std::list<UmbralMeshPtr> MeshList;

	void					InitializeMap(const MapLayoutPtr&);
	InstanceInfoMap			GetInstancesToBuild(const MapLayoutPtr&);
	void					CreateInstances(const MapLayoutPtr&, const InstanceInfoMap&);

	UmbralModelPtr			CreateBgPartObject(const MapLayoutPtr&, const std::shared_ptr<CMapLayout::BGPARTS_BASE_OBJECT_NODE>&);

	BgPartObjectMap			m_bgPartObjects;
	MeshList				m_instances;

	std::thread				m_initThread;
	std::mutex				m_instancesMutex;
	bool					m_loadingCanceled = false;
};

typedef std::shared_ptr<CUmbralMap> UmbralMapPtr;
