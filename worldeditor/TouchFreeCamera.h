#pragma once

#include "PalleonEngine.h"

class CTouchFreeCamera;
typedef std::shared_ptr<CTouchFreeCamera> TouchFreeCameraPtr;

class CTouchFreeCamera : public Palleon::CCamera
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

	void						BeginMoveForward();
	void						EndMoveForward();

	void						BeginMoveBackward();
	void						EndMoveBackward();

	void						BeginStrafeLeft();
	void						EndStrafeLeft();

	void						BeginStrafeRight();
	void						EndStrafeRight();

	void						BeginMoveUp();
	void						EndMoveUp();

	void						BeginMoveDown();
	void						EndMoveDown();

	void						BeginDrag(const CVector2&);
	void						UpdateDrag(const CVector2&);
	void						EndDrag();

	void						CancelInputTracking();

private:
	bool						m_isMovingForward = false;
	bool						m_isMovingBackward = false;
	bool						m_isStrafingLeft = false;
	bool						m_isStrafingRight = false;
	bool						m_isMovingUp = false;
	bool						m_isMovingDown = false;
	bool						m_isDragging = false;

	float						m_cameraHAngle = 0;
	float						m_dragHAngle = 0;

	float						m_cameraVAngle = 0;
	float						m_dragVAngle = 0;

	CVector3					m_cameraPosition = CVector3(0, 0, 0);

	CVector2					m_mousePosition = CVector2(0, 0);
	CVector2					m_dragPosition = CVector2(0, 0);
};
