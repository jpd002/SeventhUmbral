#pragma once

#include "PalleonEngine.h"

class CUmbralActor : public Palleon::CSceneNode
{
public:
					CUmbralActor();
	virtual			~CUmbralActor();

	void			SetBaseModelId(uint32);

private:
	void			RebuildActorRenderables();

	uint32			m_baseModelId = 0;
	uint32			m_topModelId = 0x400;
};
