#include "TouchFreeCamera.h"

#define MOVE_SPEED	(20.f)

CTouchFreeCamera::CTouchFreeCamera()
: m_commandMode(COMMAND_MODE_IDLE)
, m_cameraPosition(0, 0, 0)
, m_cameraHAngle(0)
, m_cameraVAngle(0)
, m_dragHAngle(0)
, m_dragVAngle(0)
, m_mousePosition(0, 0)
, m_dragPosition(0, 0)
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
	if(m_commandMode == COMMAND_MODE_DRAG_CAMERA)
	{
		float deltaX = m_dragPosition.x - m_mousePosition.x;
		float deltaY = m_dragPosition.y - m_mousePosition.y;
		m_cameraHAngle = m_dragHAngle - deltaX * 0.015f;
		m_cameraVAngle = m_dragVAngle - deltaY * 0.015f;
		m_cameraVAngle = std::min<float>(m_cameraVAngle, M_PI / 2);
		m_cameraVAngle = std::max<float>(m_cameraVAngle, -M_PI / 2);
	}
	else if(m_commandMode == COMMAND_MODE_MOVE_FORWARD || m_commandMode == COMMAND_MODE_MOVE_BACKWARD)
	{
		CMatrix4 yawMatrix(CMatrix4::MakeAxisYRotation(m_cameraHAngle));
		CMatrix4 pitchMatrix(CMatrix4::MakeAxisXRotation(m_cameraVAngle));
		CMatrix4 totalMatrix = yawMatrix * pitchMatrix;
		CVector3 forwardVector = CVector3(0, 0, 1) * totalMatrix;
		float direction = (m_commandMode == COMMAND_MODE_MOVE_BACKWARD) ? (-1.0f) : (1.0f);
		m_cameraPosition += (forwardVector * direction * MOVE_SPEED * dt);
	}

	CMatrix4 yawMatrix(CMatrix4::MakeAxisYRotation(m_cameraHAngle));
	CMatrix4 pitchMatrix(CMatrix4::MakeAxisXRotation(m_cameraVAngle));
	CMatrix4 translationMatrix(CMatrix4::MakeTranslation(-m_cameraPosition.x, -m_cameraPosition.y, -m_cameraPosition.z));

	CMatrix4 rotationMatrix = yawMatrix * pitchMatrix;
	CMatrix4 totalMatrix = translationMatrix * rotationMatrix;

	SetViewMatrix(totalMatrix);
}

void CTouchFreeCamera::SetPosition(const CVector3& position)
{
	m_cameraPosition = position;
}

void CTouchFreeCamera::NotifyMouseMove(unsigned int x, unsigned int y)
{
	m_mousePosition = CVector2(x, y);
}

void CTouchFreeCamera::NotifyMouseDown_MoveForward()
{
	m_commandMode = COMMAND_MODE_MOVE_FORWARD;
}

void CTouchFreeCamera::NotifyMouseDown_MoveBackward()
{
	m_commandMode = COMMAND_MODE_MOVE_BACKWARD;
}

void CTouchFreeCamera::NotifyMouseDown_Center()
{
	m_commandMode = COMMAND_MODE_DRAG_CAMERA;
	m_dragHAngle = m_cameraHAngle;
	m_dragVAngle = m_cameraVAngle;
	m_dragPosition = m_mousePosition;
}

void CTouchFreeCamera::NotifyMouseUp()
{
	m_commandMode = COMMAND_MODE_IDLE;
}
