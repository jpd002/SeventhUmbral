#include "ViewerControlScheme.h"

CViewerControlScheme::CViewerControlScheme(CWorldEditor& parent)
: m_parent(parent)
{

}

CViewerControlScheme::~CViewerControlScheme()
{

}

void CViewerControlScheme::PreUpdate(float dt)
{
	if(m_commandMode == COMMAND_MODE_DRAG_CAMERA)
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

	auto actorRotationNode = m_parent.GetActorRotationNode();
	actorRotationNode->SetRotation(rotationMatrix);

	auto actor = m_parent.GetActor(m_actorId);
	auto boundingSphere = actor->GetBoundingSphere();
	actor->SetPosition(boundingSphere.position * -1.0f);

	UpdateCameraLookAt();
}

void CViewerControlScheme::NotifyActorChanged(uint32 actorId)
{
	m_actorId = actorId;
	m_cameraHAngle = 0;
	m_cameraVAngle = 0;
	m_cameraZoomDelta = 0;

	const auto& actors = m_parent.GetActors();
	auto actorIterator = actors.find(m_actorId);
	assert(actorIterator != std::end(actors));
	auto actor = actorIterator->second;
	auto actorParent = actor->GetParent();
	auto actorRotationNode = m_parent.GetActorRotationNode();
	if(actorParent != actorRotationNode.get())
	{
		assert(actorRotationNode->GetParent() == actorParent);
		actorParent->RemoveChild(actor);
		actorRotationNode->AppendChild(actor);
	}

	UpdateCameraLookAt();
}

void CViewerControlScheme::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
}

void CViewerControlScheme::NotifyMouseDown()
{
	m_commandMode = COMMAND_MODE_DRAG_CAMERA;
	m_dragHAngle = m_cameraHAngle;
	m_dragVAngle = m_cameraVAngle;
	m_dragPosition = m_mousePosition;
}

void CViewerControlScheme::NotifyMouseUp()
{
	m_commandMode = COMMAND_MODE_IDLE;
}

void CViewerControlScheme::NotifyMouseWheel(int delta)
{
	m_cameraZoomDelta -= static_cast<float>(delta);
	m_cameraZoomDelta = std::min(m_cameraZoomDelta,  1024.f);
	m_cameraZoomDelta = std::max(m_cameraZoomDelta, -1024.f);
	UpdateCameraLookAt();
}

void CViewerControlScheme::UpdateCameraLookAt()
{
	const auto& actors = m_parent.GetActors();
	auto actorIterator = actors.find(m_actorId);
	if(actorIterator == std::end(actors)) return;

	auto camera = m_parent.GetMainCamera();
	auto actor = actorIterator->second;
	auto boundingSphere = actor->GetBoundingSphere();
	float zoomLevel = boundingSphere.radius * 2;
	float zoomLevelFactor = (m_cameraZoomDelta / 1024.f) + 1.10f;
	camera->LookAt(CVector3(0, 0, zoomLevel * zoomLevelFactor), CVector3(0, 0, 0),
		CVector3(0, 1, 0), Palleon::HANDEDNESS_RIGHTHANDED);
}
