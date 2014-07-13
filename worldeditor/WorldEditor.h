#pragma once

#include "PalleonEngine.h"
#include "TouchFreeCamera.h"
#include "../dataobjects/ResourceDefs.h"

class CWorldEditor : public Palleon::CApplication
{
public:
							CWorldEditor();
	virtual					~CWorldEditor();

	virtual void			Update(float) override;

	virtual void			NotifySizeChanged() override;

	virtual void			NotifyMouseMove(int, int) override;
	virtual void			NotifyMouseDown() override;
	virtual void			NotifyMouseUp() override;

	virtual void			NotifyKeyDown(Palleon::KEY_CODE) override;
	virtual void			NotifyKeyUp(Palleon::KEY_CODE) override;

	virtual void			NotifyInputCancelled() override;

	virtual std::string		NotifyExternalCommand(const std::string&) override;

private:
	typedef std::vector<Palleon::SceneNodePtr> ModelArray;
	typedef std::vector<Palleon::TexturePtr> TextureArray;

	void					CreateUi();
	void					CreateWorld();
	void					CreateActors();

	void					CreateMap();
	void					CreateBaseAxis();

	CVector2				m_mousePosition;

	Palleon::PackagePtr		m_package;

	Palleon::ViewportPtr	m_mainViewport;
	Palleon::ViewportPtr	m_overlayViewport;
	TouchFreeCameraPtr		m_mainCamera;
	
	Palleon::ViewportPtr	m_uiViewport;
	Palleon::LabelPtr		m_positionLabel;
	Palleon::LabelPtr		m_metricsLabel;

	float					m_elapsed;
};
