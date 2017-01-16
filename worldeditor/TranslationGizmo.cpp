#include "TranslationGizmo.h"

static const CVector3 g_arrowScale(0.075f, 0.25f, 0.075f);

static const CColor g_arrowColors[3] =
{
	CColor(1, 0, 0, 1),
	CColor(0, 1, 0, 1),
	CColor(0, 0, 1, 1)
};

static const CVector3 g_arrowPositions[3] =
{
	CVector3(1, 0, 0),
	CVector3(0, 1, 0),
	CVector3(0, 0, 1)
};

static const CQuaternion g_arrowRotations[3] =
{
	CQuaternion(CVector3(0, 0, 1), M_PI / 2.f),
	CQuaternion(),
	CQuaternion(CVector3(1, 0, 0), -M_PI / 2.f)
};

CTranslationGizmo::CTranslationGizmo()
{	
	{
		auto baseAxisNode = Palleon::CSceneNode::Create();
		baseAxisNode->SetPosition(CVector3(0.f, 0.f, 0.f));
		AppendChild(baseAxisNode);

		{
			auto axisMesh = Palleon::CAxisMesh::Create();
			axisMesh->SetScale(CVector3(1, 1, 1));
			baseAxisNode->AppendChild(axisMesh);
		}

		for(unsigned int i = 0; i < 3; i++)
		{
			auto coneMesh = Palleon::CConeMesh::Create();
			coneMesh->SetPosition(g_arrowPositions[i]);
			coneMesh->SetRotation(g_arrowRotations[i]);
			coneMesh->SetScale(g_arrowScale);
			coneMesh->GetMaterial()->SetColor(g_arrowColors[i]);
			baseAxisNode->AppendChild(coneMesh);
			m_arrows[i] = coneMesh;
		}
	}
}

CTranslationGizmo::~CTranslationGizmo()
{

}

TranslationGizmoPtr CTranslationGizmo::Create()
{
	return std::make_shared<CTranslationGizmo>();
}

CTranslationGizmo::HITTEST_RESULT CTranslationGizmo::HitTest(const CRay& ray)
{
	//Test arrows
	for(unsigned int i = 0; i < 3; i++)
	{
		const auto& arrow = m_arrows[i];
		auto boundingSphere = arrow->GetWorldBoundingSphere();
		auto intersectResult = Intersects(boundingSphere, ray);
		if(intersectResult.first)
		{
			switch(i)
			{
			case 0:		return HITTEST_X;
			case 1:		return HITTEST_Y;
			case 2:		return HITTEST_Z;
			default:	return HITTEST_NONE;
			}
		}
	}
	return HITTEST_NONE;
}

void CTranslationGizmo::UpdateHoverState(const CRay& ray)
{
	for(unsigned int i = 0; i < 3; i++)
	{
		const auto& arrow = m_arrows[i];
		arrow->GetMaterial()->SetColor(g_arrowColors[i]);
	}
	if(m_stickyMode != HITTEST_NONE)
	{
		switch(m_stickyMode)
		{
		case HITTEST_X:
			m_arrows[0]->GetMaterial()->SetColor(CColor(1, 1, 1, 1));
			break;
		case HITTEST_Y:
			m_arrows[1]->GetMaterial()->SetColor(CColor(1, 1, 1, 1));
			break;
		case HITTEST_Z:
			m_arrows[2]->GetMaterial()->SetColor(CColor(1, 1, 1, 1));
			break;
		default:
			assert(0);
			break;
		}
	}
	else
	{
		for(unsigned int i = 0; i < 3; i++)
		{
			const auto& arrow = m_arrows[i];
			auto boundingSphere = arrow->GetWorldBoundingSphere();
			auto intersectResult = Intersects(boundingSphere, ray);
			if(intersectResult.first)
			{
				arrow->GetMaterial()->SetColor(CColor(1, 1, 1, 1));
			}
		}
	}
}

void CTranslationGizmo::SetStickyMode(HITTEST_RESULT stickyMode)
{
	m_stickyMode = stickyMode;
}
