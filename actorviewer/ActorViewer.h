#pragma once

#include "PalleonEngine.h"
#include "TouchFreeCamera.h"

class CActorViewer : public Palleon::CApplication
{
public:
							CActorViewer();
	virtual					~CActorViewer();

	virtual void			Update(float) override;

	virtual void			NotifyMouseMove(int, int) override;
	virtual void			NotifyMouseDown() override;
	virtual void			NotifyMouseUp() override;

private:
	typedef std::vector<Palleon::SceneNodePtr> ModelArray;
	typedef std::vector<Palleon::TexturePtr> TextureArray;

	void					CreateUi();
	void					CreateActor();
	void					UpdateLights();

	CVector2				m_mousePosition;

	Palleon::PackagePtr		m_package;

	Palleon::ViewportPtr	m_mainViewport;
	TouchFreeCameraPtr		m_mainCamera;
	
	Palleon::ViewportPtr	m_uiViewport;
	Palleon::LabelPtr		m_positionLabel;
	Palleon::LabelPtr		m_metricsLabel;

	float					m_elapsed = 0;

	CBox2					m_forwardButtonBoundingBox;
	CBox2					m_backwardButtonBoundingBox;
};
