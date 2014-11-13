#pragma once

#include "PalleonEngine.h"

class CTranslationGizmo;
typedef std::shared_ptr<CTranslationGizmo> TranslationGizmoPtr;

class CTranslationGizmo : public Palleon::CSceneNode
{
public:
	enum HITTEST_RESULT
	{
		HITTEST_NONE,
		HITTEST_X,
		HITTEST_Y,
		HITTEST_Z
	};

								CTranslationGizmo();
	virtual						~CTranslationGizmo();

	static TranslationGizmoPtr	Create();

	HITTEST_RESULT				HitTest(const CRay&);
	void						UpdateHoverState(const CRay&);

	void						SetStickyMode(HITTEST_RESULT);

private:
	HITTEST_RESULT				m_stickyMode = HITTEST_NONE;
	Palleon::MeshPtr			m_arrows[3];
};
