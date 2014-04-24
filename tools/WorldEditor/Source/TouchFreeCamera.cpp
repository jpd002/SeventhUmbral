#include "TouchFreeCamera.h"
#include <assert.h>

#define MOVE_SPEED	(10.f)

CTouchFreeCamera::CTouchFreeCamera()
{

}

CTouchFreeCamera::~CTouchFreeCamera()
{

}

TouchFreeCameraPtr CTouchFreeCamera::Create()
{
	return std::make_shared<CTouchFreeCamera>();
}

void CTouchFreeCamera::Update(float dt)
{
	if(m_isDragging)
	{
		float deltaX = m_dragPosition.x - m_mousePosition.x;
		float deltaY = m_dragPosition.y - m_mousePosition.y;
		m_cameraHAngle = m_dragHAngle + deltaX * 0.015f;
		m_cameraVAngle = m_dragVAngle + deltaY * 0.015f;
		m_cameraVAngle = std::min<float>(m_cameraVAngle, M_PI / 2);
		m_cameraVAngle = std::max<float>(m_cameraVAngle, -M_PI / 2);
	}

	CMatrix4 yawMatrix(CMatrix4::MakeAxisYRotation(m_cameraHAngle));
	CMatrix4 pitchMatrix(CMatrix4::MakeAxisXRotation(m_cameraVAngle));
	CMatrix4 rotationMatrix = yawMatrix * pitchMatrix;

	if(m_isStrafingLeft || m_isStrafingRight)
	{
		CVector3 rightVector = CVector3(1, 0, 0) * rotationMatrix;
		float direction = m_isStrafingLeft ? (-1.0f) : (1.0f);
		m_cameraPosition += (rightVector * direction * MOVE_SPEED * dt);
	}
	if(m_isMovingForward || m_isMovingBackward)
	{
		CVector3 forwardVector = CVector3(0, 0, 1) * rotationMatrix;
		float direction = m_isMovingForward ? (-1.0f) : (1.0f);
		m_cameraPosition += (forwardVector * direction * MOVE_SPEED * dt);
	}

	CMatrix4 translationMatrix(CMatrix4::MakeTranslation(-m_cameraPosition.x, -m_cameraPosition.y, -m_cameraPosition.z));
	CMatrix4 totalMatrix = translationMatrix * rotationMatrix;

	SetViewMatrix(totalMatrix);
}

CVector3 CTouchFreeCamera::GetPosition() const
{
	return m_cameraPosition;
}

void CTouchFreeCamera::SetPosition(const CVector3& position)
{
	m_cameraPosition = position;
}

void CTouchFreeCamera::SetHorizontalAngle(float hAngle)
{
	m_cameraHAngle = hAngle;
}

void CTouchFreeCamera::SetVerticalAngle(float vAngle)
{
	m_cameraVAngle = vAngle;
}

void CTouchFreeCamera::BeginMoveForward()
{
	assert(!m_isMovingForward);
	m_isMovingForward = true;
}

void CTouchFreeCamera::EndMoveForward()
{
	assert(m_isMovingForward);
	m_isMovingForward = false;
}

void CTouchFreeCamera::BeginMoveBackward()
{
	assert(!m_isMovingBackward);
	m_isMovingBackward = true;
}

void CTouchFreeCamera::EndMoveBackward()
{
	assert(m_isMovingBackward);
	m_isMovingBackward = false;
}

void CTouchFreeCamera::BeginStrafeLeft()
{
	assert(!m_isStrafingLeft);
	m_isStrafingLeft = true;
}

void CTouchFreeCamera::EndStrafeLeft()
{
	assert(m_isStrafingLeft);
	m_isStrafingLeft = false;
}

void CTouchFreeCamera::BeginStrafeRight()
{
	assert(!m_isStrafingRight);
	m_isStrafingRight = true;
}

void CTouchFreeCamera::EndStrafeRight()
{
	assert(m_isStrafingRight);
	m_isStrafingRight = false;
}

void CTouchFreeCamera::BeginDrag(const CVector2& mousePosition)
{
	assert(!m_isDragging);
	m_isDragging = true;
	m_dragHAngle = m_cameraHAngle;
	m_dragVAngle = m_cameraVAngle;
	m_dragPosition = mousePosition;
}

void CTouchFreeCamera::UpdateDrag(const CVector2& mousePosition)
{
	m_mousePosition = mousePosition;
}

void CTouchFreeCamera::EndDrag()
{
	assert(m_isDragging);
	m_isDragging = false;
}

void CTouchFreeCamera::CancelInputTracking()
{
	m_isDragging = false;
	m_isMovingForward = false;
	m_isMovingBackward = false;
	m_isStrafingLeft = false;
	m_isStrafingRight = false;
}
