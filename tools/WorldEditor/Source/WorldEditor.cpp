#include "WorldEditor.h"
#include "StdStream.h"
#include "StdStreamUtils.h"
#include "SectionLoader.h"
#include "UmbralModel.h"

CWorldEditor::CWorldEditor()
: m_elapsed(0)
{
	auto screenSize = Athena::CGraphicDevice::GetInstance().GetScreenSize();

	{
		auto camera = Athena::CCamera::Create();
		camera->SetPerspectiveProjection(M_PI / 4.f, screenSize.x / screenSize.y, 1.f, 1000.f);
		camera->LookAt(CVector3(0, 0, 100.f), CVector3(0, 0, 0), CVector3(0, 1, 0));
		m_mainCamera = camera;
	}

	{
		auto camera = Athena::CCamera::Create();
//		camera->SetOrthoProjection(2048 / 16, 2048 / 16, 100.f, 300.f);
		camera->SetPerspectiveProjection(M_PI / 4.f, 1, 100.f, 300.f);
		m_shadowCamera = camera;
	}
	
	{
		auto viewport = Athena::CViewport::Create();
		viewport->SetCamera(m_mainCamera);
		viewport->SetShadowCamera(m_shadowCamera);
		m_viewport = viewport;
	}

	{
		auto sphere = Athena::CSphereMesh::Create();
		sphere->SetScale(CVector3(4, 4, 4));
		sphere->SetPosition(CVector3(0, 0, 0));
		sphere->GetMaterial()->SetShadowCasting(true);
		sphere->GetMaterial()->SetColor(CColor(1, 0, 0, 1));
		m_viewport->GetSceneRoot()->AppendChild(sphere);
		m_sphere = sphere;
	}

	{
		auto cube = Athena::CCubeMesh::Create();
		cube->SetPosition(CVector3(30, -20, 0));
		cube->SetScale(CVector3(10, 10, 10));
		cube->GetMaterial()->SetShadowCasting(true);
		cube->GetMaterial()->SetColor(CColor(0, 1, 1, 1));
		m_viewport->GetSceneRoot()->AppendChild(cube);
	}

	{
//		auto inputStream = Framework::CreateInputStdStream(std::string("F:\\Games\\SquareEnix\\FINAL FANTASY XIV\\client\\chara\\bgobj\\b901\\equ\\e001\\top_mdl\\0001"));
		auto inputStream = Framework::CreateInputStdStream(std::string("F:\\Games\\SquareEnix\\FINAL FANTASY XIV\\client\\chara\\mon\\m003\\equ\\e001\\top_mdl\\0001"));
		auto resourceNode = CSectionLoader::ReadSection(inputStream);
		auto modelNodes = resourceNode->SelectNodes<CModelChunk>();
		auto model = std::make_shared<CUmbralModel>(modelNodes[0]);
		model->SetScale(CVector3(10, 10, 10));
		model->TraverseNodes(
			[] (const Athena::SceneNodePtr& node) 
			{
				if(auto mesh = std::dynamic_pointer_cast<Athena::CMesh>(node))
				{
					mesh->GetMaterial()->SetShadowCasting(true);
				}
				return true;
			}
		);
		m_viewport->GetSceneRoot()->AppendChild(model);
	}

	//Ground
	{
		auto cube = Athena::CCubeMesh::Create();
		cube->SetPosition(CVector3(0, -40, 0));
		cube->SetScale(CVector3(100, 10, 100));
		cube->GetMaterial()->SetColor(CColor(0, 0, 1, 1));
		cube->GetMaterial()->SetShadowReceiving(true);
		m_viewport->GetSceneRoot()->AppendChild(cube);
		m_cube = cube;
	}

	Athena::CGraphicDevice::GetInstance().AddViewport(m_viewport.get());
}

CWorldEditor::~CWorldEditor()
{
	Athena::CGraphicDevice::GetInstance().RemoveViewport(m_viewport.get());
}

void CWorldEditor::Update(float dt)
{
	float angle = m_elapsed;
	auto cameraPosition = CVector3(100.f * sin(angle), 200.f, 100.f * cos(angle));
	auto cameraDir = cameraPosition.Normalize();
	auto cameraUp = CVector3(cameraDir.x, -cameraDir.y, cameraDir.z);
	m_shadowCamera->LookAt(cameraPosition, CVector3(0, 0, 0), cameraUp);
//	m_shadowCamera->LookAt(CVector3(0, 100, 0), CVector3(0, 0, 0), CVector3(1, 0, 0));
	m_viewport->GetSceneRoot()->Update(dt);
	m_viewport->GetSceneRoot()->UpdateTransformations();
	m_elapsed += dt;
}

Athena::CApplication* CreateApplication()
{
	return new CWorldEditor();
}
