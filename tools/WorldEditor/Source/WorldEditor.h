#pragma once

#include "AthenaEngine.h"

class CWorldEditor : public Athena::CApplication
{
public:
							CWorldEditor();
	virtual					~CWorldEditor();

	virtual void			Update(float) override;

private:
	Athena::ViewportPtr		m_viewport;
	Athena::CameraPtr		m_mainCamera;
	Athena::CameraPtr		m_shadowCamera;

	Athena::MeshPtr			m_cube;
	Athena::MeshPtr			m_sphere;

	float					m_elapsed;
};
