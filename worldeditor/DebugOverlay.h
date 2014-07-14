#pragma once

#include "PalleonEngine.h"

class CDebugOverlay
{
public:
							CDebugOverlay();
	virtual					~CDebugOverlay();

	void					SetCameraPosition(const CVector3&);
	void					Update(float);

private:
	void					CreateUi();

	Palleon::PackagePtr		m_package;

	Palleon::ViewportPtr	m_uiViewport;

	Palleon::LabelPtr		m_positionLabel;
	Palleon::LabelPtr		m_metricsLabel;
	
	CVector3				m_cameraPosition = CVector3(0, 0, 0);
};

typedef std::shared_ptr<CDebugOverlay> DebugOverlayPtr;
