#pragma once

#include "PalleonEngine.h"

class CDebugOverlay
{
public:
							CDebugOverlay();
	virtual					~CDebugOverlay();

	void					Update(float);

	void					NotifySizeChanged();

private:
	void					CreateUi();

	Palleon::PackagePtr		m_package;

	Palleon::ViewportPtr	m_uiViewport;
	Palleon::LabelPtr		m_positionLabel;
	Palleon::LabelPtr		m_metricsLabel;
};

typedef std::shared_ptr<CDebugOverlay> DebugOverlayPtr;
