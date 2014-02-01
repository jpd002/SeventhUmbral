#pragma once

#include "AthenaEngine.h"

class CTouchFreeCamera;
typedef std::shared_ptr<CTouchFreeCamera> TouchFreeCameraPtr;

class CTouchFreeCamera : public Athena::CCamera
{
public:
								CTouchFreeCamera();
	virtual						~CTouchFreeCamera();

	static TouchFreeCameraPtr	Create();

	virtual void				Update(float);

	CVector3					GetPosition() const;
	void						SetPosition(const CVector3&);
	
	void						SetHorizontalAngle(float);
	void						SetVerticalAngle(float);

	void						NotifyMouseMove(unsigned int, unsigned int);
	void						NotifyMouseDown_MoveForward();
	void						NotifyMouseDown_MoveBackward();
	void						NotifyMouseDown_Center();
	void						NotifyMouseUp();

private:
	enum COMMAND_MODE
	{
		COMMAND_MODE_IDLE			= 0,
		COMMAND_MODE_DRAG_CAMERA	= 1,
		COMMAND_MODE_MOVE_FORWARD	= 2,
		COMMAND_MODE_MOVE_BACKWARD	= 3,
	};

	COMMAND_MODE		m_commandMode;

	float				m_cameraHAngle;
	float				m_dragHAngle;

	float				m_cameraVAngle;
	float				m_dragVAngle;

	CVector3			m_cameraPosition;

	CVector2			m_mousePosition;
	CVector2			m_dragPosition;
};
