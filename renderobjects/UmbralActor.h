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
	void			SetHelmModelId(uint32);
	void			SetTopModelId(uint32);
	void			SetBottomModelId(uint32);
	void			SetGloveModelId(uint32);
	void			SetShoeModelId(uint32);

	void			RebuildActorRenderables();

	virtual void	Update(float) override;

private:
	enum ACTOR_TYPE
	{
		ACTOR_PC		= 0,
		ACTOR_MONSTER	= 1,
		ACTOR_BGOBJ		= 2,
		ACTOR_WEAPON	= 4,
	};

	enum SUBMODEL_TYPE
	{
		SUBMODEL_HELM,
		SUBMODEL_TOP,
		SUBMODEL_BOTTOM,
		SUBMODEL_GLOVE,
		SUBMODEL_SHOE,
	};

	void			LoadSubModel(SUBMODEL_TYPE, uint32);

	UmbralModelPtr	CreateModel(const ModelChunkPtr&);

	uint32			ComputeVarWepId(uint32, uint32, uint32, uint32);
	uint32			ComputeVarEquipId(ACTOR_TYPE, SUBMODEL_TYPE, uint32, uint32, uint32);

	uint32			m_baseModelId	= 0;
	uint32			m_helmModelId	= 0x400;
	uint32			m_topModelId	= 0x400;
	uint32			m_bottomModelId	= 0x400;
	uint32			m_gloveModelId	= 0x400;
	uint32			m_shoeModelId	= 0x400;

	bool			m_renderableDirty = true;
	
	CSphere			m_boundingSphere;
};

typedef std::shared_ptr<CUmbralActor> UmbralActorPtr;
