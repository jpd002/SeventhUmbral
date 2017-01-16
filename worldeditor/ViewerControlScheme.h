#pragma once

#include "ControlScheme.h"
#include "TranslationGizmo.h"
#include "WorldEditor.h"

class CViewerControlScheme : public CControlScheme
{
public:
							CViewerControlScheme(CWorldEditor&);
	virtual					~CViewerControlScheme();

	virtual void			PreUpdate(float) override;

	virtual void			NotifyActorChanged(uint32) override;

	virtual void			NotifyMouseMove(int, int) override;
	virtual void			NotifyMouseDown() override;
	virtual void			NotifyMouseUp() override;
	virtual void			NotifyMouseWheel(int) override;

private:
	enum COMMAND_MODE
	{
		COMMAND_MODE_IDLE			= 0,
		COMMAND_MODE_DRAG_CAMERA	= 1,
	};

	void					UpdateCameraLookAt();

	CWorldEditor&			m_parent;

	uint32					m_actorId = 0;

	COMMAND_MODE			m_commandMode = COMMAND_MODE_IDLE;

	float					m_cameraHAngle = 0;
	float					m_dragHAngle = 0;

	float					m_cameraVAngle = 0;
	float					m_dragVAngle = 0;

	float					m_cameraZoomDelta = 0.f;

	CVector2				m_mousePosition = CVector2(0, 0);
	CVector2				m_dragPosition = CVector2(0, 0);
};
