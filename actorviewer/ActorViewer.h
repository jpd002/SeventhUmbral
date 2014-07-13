#pragma once

#include "PalleonEngine.h"
#include "../renderobjects/UmbralActor.h"
#include "DebugOverlay.h"

class CActorViewer : public Palleon::CApplication
{
public:
							CActorViewer(bool);
	virtual					~CActorViewer();

	virtual void			Update(float) override;

	virtual void			NotifySizeChanged() override;

	virtual void			NotifyMouseMove(int, int) override;
	virtual void			NotifyMouseWheel(int) override;
	virtual void			NotifyMouseDown() override;
	virtual void			NotifyMouseUp() override;

	virtual std::string		NotifyExternalCommand(const std::string&) override;

private:
	enum COMMAND_MODE
	{
		COMMAND_MODE_IDLE			= 0,
		COMMAND_MODE_DRAG_CAMERA	= 1,
	};

	typedef std::vector<Palleon::SceneNodePtr> ModelArray;
	typedef std::vector<Palleon::TexturePtr> TextureArray;

	void					CreateScene();
	void					CreateUi();
	void					SetActor(uint32, uint32);
	void					UpdateLights();
	void					UpdateCameraLookAt();

	Palleon::ViewportPtr	m_mainViewport;
	Palleon::CameraPtr		m_mainCamera;
	Palleon::SceneNodePtr	m_actorRotationNode;
	UmbralActorPtr			m_actor;

	DebugOverlayPtr			m_debugOverlay;

	float					m_elapsed = 0;

	//Actor Rotation/////
	COMMAND_MODE			m_commandMode = COMMAND_MODE_IDLE;

	float					m_cameraHAngle = 0;
	float					m_dragHAngle = 0;

	float					m_cameraVAngle = 0;
	float					m_dragVAngle = 0;

	float					m_cameraZoomDelta = 0.f;
	///////////////////////////////////

	CVector2				m_mousePosition = CVector2(0, 0);
	CVector2				m_dragPosition = CVector2(0, 0);
};
