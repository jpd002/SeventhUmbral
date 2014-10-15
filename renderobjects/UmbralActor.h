#pragma once

#include "PalleonEngine.h"
#include "UmbralModel.h"
#include "../dataobjects/ResourceDefs.h"

class CUmbralActor : public Palleon::CSceneNode
{
public:
					CUmbralActor();
	virtual			~CUmbralActor();

	const CSphere&	GetBoundingSphere() const;

	void			SetBaseModelId(uint32);
	void			SetTopModelId(uint32);

	void			RebuildActorRenderables();

	virtual void	Update(float) override;

private:
	UmbralModelPtr	CreateModel(const ModelChunkPtr&);

	uint32			m_baseModelId = 0;
	uint32			m_topModelId = 0x400;

	bool			m_renderableDirty = true;
	
	CSphere			m_boundingSphere;
};

typedef std::shared_ptr<CUmbralActor> UmbralActorPtr;
