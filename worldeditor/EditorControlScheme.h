#pragma once

#include "ControlScheme.h"
#include "TranslationGizmo.h"
#include "WorldEditor.h"

class CEditorControlScheme : public CControlScheme
{
public:
					CEditorControlScheme(CWorldEditor&);
	virtual			~CEditorControlScheme();

	virtual void	PreUpdate(float) override;
	virtual void	PostUpdate(float) override;

	virtual void	NotifyMouseMove(int, int) override;
	virtual void	NotifyMouseDown() override;
	virtual void	NotifyMouseUp() override;

	virtual void	NotifyKeyDown(Palleon::KEY_CODE) override;
	virtual void	NotifyKeyUp(Palleon::KEY_CODE) override;

private:
	enum STATE
	{
		STATE_IDLE,
		STATE_TRANSLATE,
	};

	CRay								GetMouseRay() const;

	Palleon::SceneNodePtr				m_selectedActorNode;
	uint32								m_selectedActorId = 0;

	CWorldEditor&						m_parent;

	CVector2							m_mousePosition = CVector2(0, 0);
	STATE								m_state = STATE_IDLE;
	CTranslationGizmo::HITTEST_RESULT	m_translationMode = CTranslationGizmo::HITTEST_NONE;
	CVector3							m_intersectPosition = CVector3(0, 0, 0);
	CVector3							m_lastIntersectPosition = CVector3(0, 0, 0);
	CVector3							m_delta = CVector3(0, 0, 0);
	float								m_dot[2];
};
