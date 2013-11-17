#pragma once

#include "AthenaEngine.h"
#include "TouchFreeCamera.h"
#include "MapLayout.h"

class CWorldEditor : public Athena::CApplication
{
public:
							CWorldEditor();
	virtual					~CWorldEditor();

	virtual void			Update(float) override;

	virtual void			NotifyMouseMove(unsigned int, unsigned int) override;
	virtual void			NotifyMouseDown() override;
	virtual void			NotifyMouseUp() override;

private:
	typedef std::vector<Athena::SceneNodePtr> ModelArray;
	typedef std::vector<Athena::TexturePtr> TextureArray;

	void					CreateUi();
	void					CreateWorld();
	
	Athena::SceneNodePtr	CreateUnitTreeObject(const std::shared_ptr<CMapLayout::UNIT_TREE_OBJECT_NODE>&);

	CVector2				m_mousePosition;

	Athena::PackagePtr		m_package;

	Athena::ViewportPtr		m_mainViewport;
//	Athena::CameraPtr		m_mainCamera;
	TouchFreeCameraPtr		m_mainCamera;
	
	Athena::ViewportPtr		m_uiViewport;

	MapLayoutPtr			m_mapLayout;

	float					m_elapsed;

	CBox2					m_forwardButtonBoundingBox;
	CBox2					m_backwardButtonBoundingBox;
};
