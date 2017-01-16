#include "EditorControlScheme.h"
#include "math/MathStringUtils.h"

static const CVector3 g_translateXPlaneNormals[2] =
{
	CVector3(0, 1, 0),
	CVector3(0, 0, 1),
};

static const CVector3 g_translateYPlaneNormals[2] =
{
	CVector3(1, 0, 0),
	CVector3(0, 0, 1)
};

static const CVector3 g_translateZPlaneNormals[2] =
{
	CVector3(1, 0, 0),
	CVector3(0, 1, 0)
};

CEditorControlScheme::CEditorControlScheme(CWorldEditor& parent)
: m_parent(parent)
{

}

CEditorControlScheme::~CEditorControlScheme()
{

}

void CEditorControlScheme::PreUpdate(float dt)
{
	auto mainCamera = m_parent.GetMainCamera();
	mainCamera->Update(dt);

	auto translationGizmo = m_parent.GetTranslationGizmo();
	translationGizmo->SetVisible(m_selectedActorNode != nullptr);
	if(m_selectedActorNode != nullptr)
	{
		translationGizmo->SetPosition(m_selectedActorNode->GetPosition());
		auto gizmoDistance = (mainCamera->GetPosition() - translationGizmo->GetPosition()).Length();
		auto gizmoScale = gizmoDistance * 0.1f;
		translationGizmo->SetScale(CVector3(gizmoScale, gizmoScale, gizmoScale));
	}
}

void CEditorControlScheme::PostUpdate(float dt)
{
	auto translationGizmo = m_parent.GetTranslationGizmo();

	if(m_selectedActorNode != nullptr)
	{
		auto mouseRay = GetMouseRay();
		translationGizmo->UpdateHoverState(mouseRay);
	}

	if(m_state == STATE_TRANSLATE)
	{
		m_lastIntersectPosition = m_intersectPosition;
		auto mouseRay = GetMouseRay();
		auto objectTransform = m_selectedActorNode->GetWorldTransformation();
		CVector3 objectPosition(objectTransform(3, 0), objectTransform(3, 1), objectTransform(3, 2));
		if(m_translationMode == CTranslationGizmo::HITTEST_X)
		{
			auto translationPlane = CPlane();
			if(fabs(m_dot[0]) > fabs(m_dot[1]))
			{
				translationPlane = CPlane(g_translateXPlaneNormals[0], objectPosition.y);
			}
			else
			{
				translationPlane = CPlane(g_translateXPlaneNormals[1], objectPosition.z);
			}
			auto intersectResult = Intersects(translationPlane, mouseRay);
			if(intersectResult.first)
			{
				m_intersectPosition = intersectResult.second;
				if(!m_lastIntersectPosition.IsNull())
				{
					m_delta = m_intersectPosition - m_lastIntersectPosition;
				}
				m_selectedActorNode->SetPosition(m_selectedActorNode->GetPosition() + CVector3(m_delta.x, 0, 0));
			}
		}
		else if(m_translationMode == CTranslationGizmo::HITTEST_Y)
		{
			auto translationPlane = CPlane();
			if(fabs(m_dot[0]) > fabs(m_dot[1]))
			{
				translationPlane = CPlane(g_translateYPlaneNormals[0], objectPosition.x);
			}
			else
			{
				translationPlane = CPlane(g_translateYPlaneNormals[1], objectPosition.z);
			}
			auto intersectResult = Intersects(translationPlane, mouseRay);
			if(intersectResult.first)
			{
				m_intersectPosition = intersectResult.second;
				if(!m_lastIntersectPosition.IsNull())
				{
					m_delta = m_intersectPosition - m_lastIntersectPosition;
				}
				m_selectedActorNode->SetPosition(m_selectedActorNode->GetPosition() + CVector3(0, m_delta.y, 0));
			}
		}
		else if(m_translationMode == CTranslationGizmo::HITTEST_Z)
		{
			auto translationPlane = CPlane();
			if(fabs(m_dot[0]) > fabs(m_dot[1]))
			{
				translationPlane = CPlane(g_translateZPlaneNormals[0], objectPosition.x);
			}
			else
			{
				translationPlane = CPlane(g_translateZPlaneNormals[1], objectPosition.y);
			}
			auto intersectResult = Intersects(translationPlane, mouseRay);
			if(intersectResult.first)
			{
				m_intersectPosition = intersectResult.second;
				if(!m_lastIntersectPosition.IsNull())
				{
					m_delta = m_intersectPosition - m_lastIntersectPosition;
				}
				m_selectedActorNode->SetPosition(m_selectedActorNode->GetPosition() + CVector3(0, 0, m_delta.z));
			}
		}
	}
}

void CEditorControlScheme::NotifyMouseMove(int x, int y)
{
	auto mainCamera = m_parent.GetMainCamera();
	m_mousePosition = CVector2(static_cast<float>(x), static_cast<float>(y));
	mainCamera->UpdateDrag(m_mousePosition);
}

void CEditorControlScheme::NotifyMouseDown()
{
	auto translationGizmo = m_parent.GetTranslationGizmo();
//	m_mainCamera->BeginDrag(m_mousePosition);

	assert(m_state == STATE_IDLE);
	if(m_selectedActorNode != nullptr)
	{
		auto mouseRay = GetMouseRay();
		auto testResult = translationGizmo->HitTest(mouseRay);
		if(testResult != CTranslationGizmo::HITTEST_NONE)
		{
			translationGizmo->SetStickyMode(testResult);
			m_state = STATE_TRANSLATE;
			m_translationMode = testResult;
			m_intersectPosition = CVector3(0, 0, 0);
			m_lastIntersectPosition = CVector3(0, 0, 0);
			m_delta = CVector3(0, 0, 0);

			switch(testResult)
			{
			case CTranslationGizmo::HITTEST_X:
				for(unsigned int i = 0; i < 2; i++)
				{
					m_dot[i] = mouseRay.direction.Dot(g_translateXPlaneNormals[i]);
				}
				break;
			case CTranslationGizmo::HITTEST_Y:
				for(unsigned int i = 0; i < 2; i++)
				{
					m_dot[i] = mouseRay.direction.Dot(g_translateYPlaneNormals[i]);
				}
				break;
			case CTranslationGizmo::HITTEST_Z:
				for(unsigned int i = 0; i < 2; i++)
				{
					m_dot[i] = mouseRay.direction.Dot(g_translateZPlaneNormals[i]);
				}
				break;
			}
		}
	}
}

void CEditorControlScheme::NotifyMouseUp()
{
	auto translationGizmo = m_parent.GetTranslationGizmo();

//	m_mainCamera->EndDrag();

	if(m_state == STATE_IDLE)
	{
		auto mouseRay = GetMouseRay();

		m_selectedActorNode.reset();

		for(const auto& actorPair : m_parent.GetActors())
		{
			const auto& actor = actorPair.second;
			auto boundingSphere = Transform(actor->GetBoundingSphere(), actor->GetWorldTransformation());
			auto intersectResult = Intersects(boundingSphere, mouseRay);
			if(intersectResult.first)
			{
				if(m_parent.GetIsEmbedding())
				{
					Palleon::CEmbedRemoteCall rpc;
					rpc.SetMethod("selected");
					rpc.SetParam("actorId", std::to_string(actorPair.first));
					Palleon::CEmbedManager::GetInstance().NotifyClient(rpc);
				}
				m_selectedActorNode = actor;
				m_selectedActorId = actorPair.first;
				break;
			}
		}

		if(m_selectedActorNode == nullptr)
		{
			if(m_parent.GetIsEmbedding())
			{
				Palleon::CEmbedRemoteCall rpc;
				rpc.SetMethod("selected");
				rpc.SetParam("actorId", std::to_string(0));
				Palleon::CEmbedManager::GetInstance().NotifyClient(rpc);
			}
		}
	}
	else if(m_state == STATE_TRANSLATE)
	{
		if(m_selectedActorNode != nullptr)
		{
			//Notify about the actor's new position
			if(m_parent.GetIsEmbedding())
			{
				auto position = m_selectedActorNode->GetPosition();
				Palleon::CEmbedRemoteCall rpc;
				rpc.SetMethod("actorMoved");
				rpc.SetParam("actorId", std::to_string(m_selectedActorId));
				rpc.SetParam("position", MathStringUtils::ToString(position));
				Palleon::CEmbedManager::GetInstance().NotifyClient(rpc);
			}
		}

		m_state = STATE_IDLE;
		translationGizmo->SetStickyMode(CTranslationGizmo::HITTEST_NONE);
		m_translationMode = CTranslationGizmo::HITTEST_NONE;
	}
	else
	{
		
	}
}

void CEditorControlScheme::NotifyKeyDown(Palleon::KEY_CODE keyCode)
{
	auto mainCamera = m_parent.GetMainCamera();
	switch(keyCode)
	{
	case Palleon::KEY_CODE_ARROW_UP:
		mainCamera->BeginMoveForward();
		break;
	case Palleon::KEY_CODE_ARROW_DOWN:
		mainCamera->BeginMoveBackward();
		break;
	case Palleon::KEY_CODE_ARROW_LEFT:
		mainCamera->BeginStrafeLeft();
		break;
	case Palleon::KEY_CODE_ARROW_RIGHT:
		mainCamera->BeginStrafeRight();
		break;
	case Palleon::KEY_CODE_S:
		mainCamera->BeginMoveUp();
		break;
	case Palleon::KEY_CODE_W:
		mainCamera->BeginMoveDown();
		break;
	}
	
}

void CEditorControlScheme::NotifyKeyUp(Palleon::KEY_CODE keyCode)
{
	auto mainCamera = m_parent.GetMainCamera();
	switch(keyCode)
	{
	case Palleon::KEY_CODE_ARROW_UP:
		mainCamera->EndMoveForward();
		break;
	case Palleon::KEY_CODE_ARROW_DOWN:
		mainCamera->EndMoveBackward();
		break;
	case Palleon::KEY_CODE_ARROW_LEFT:
		mainCamera->EndStrafeLeft();
		break;
	case Palleon::KEY_CODE_ARROW_RIGHT:
		mainCamera->EndStrafeRight();
		break;
	case Palleon::KEY_CODE_S:
		mainCamera->EndMoveUp();
		break;
	case Palleon::KEY_CODE_W:
		mainCamera->EndMoveDown();
		break;
	}
}

CRay CEditorControlScheme::GetMouseRay() const
{
	auto mainCamera = m_parent.GetMainCamera();
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	auto screenPos = (m_mousePosition / screenSize) * 2 - CVector2(1, 1);
	auto ray = mainCamera->Unproject(screenPos);
	return ray;
}
