#pragma once

#include "PalleonEngine.h"
#include "TouchFreeCamera.h"
#include "../dataobjects/ResourceDefs.h"
#include "../renderobjects/UmbralActor.h"
#include "../renderobjects/UmbralMap.h"
#include "DebugOverlay.h"
#include "TranslationGizmo.h"

class CWorldEditor : public Palleon::CApplication
{
public:
							CWorldEditor(bool);
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
	enum STATE
	{
		STATE_IDLE,
		STATE_TRANSLATE,
	};

	typedef std::map<uint32, UmbralActorPtr> ActorMap;

	void					CreateWorld();
	void					CreateActor(uint32);
	void					SetActorPosition(uint32, const CVector3&);
	void					SetActorBaseModelId(uint32, uint32);
	void					CreateMap(uint32);

	CRay					GetMouseRay() const;

	std::string				ProcessSetMap(const Palleon::CEmbedRemoteCall&);
	std::string				ProcessCreateActor(const Palleon::CEmbedRemoteCall&);
	std::string				ProcessSetActorBaseModelId(const Palleon::CEmbedRemoteCall&);
	std::string				ProcessSetActorPosition(const Palleon::CEmbedRemoteCall&);

	CVector2				m_mousePosition = CVector2(0, 0);

	bool					m_isEmbedding = false;

	Palleon::ViewportPtr	m_mainViewport;
	Palleon::ViewportPtr	m_overlayViewport;
	TouchFreeCameraPtr		m_mainCamera;
	
	UmbralMapPtr			m_map;
	ActorMap				m_actors;
	Palleon::SceneNodePtr	m_selectedActorNode;
	uint32					m_selectedActorId = 0;
	TranslationGizmoPtr		m_translationGizmo;

	DebugOverlayPtr						m_debugOverlay;
	STATE								m_state = STATE_IDLE;
	CTranslationGizmo::HITTEST_RESULT	m_translationMode = CTranslationGizmo::HITTEST_NONE;
	CVector3							m_intersectPosition = CVector3(0, 0, 0);
	CVector3							m_lastIntersectPosition = CVector3(0, 0, 0);
	CVector3							m_delta = CVector3(0, 0, 0);
	float								m_dot[2];
};
