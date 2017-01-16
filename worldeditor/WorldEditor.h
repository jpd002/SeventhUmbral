#pragma once

#include "PalleonEngine.h"
#include "TouchFreeCamera.h"
#include "../dataobjects/ResourceDefs.h"
#include "../renderobjects/UmbralActor.h"
#include "../renderobjects/UmbralMap.h"
#include "DebugOverlay.h"
#include "ControlScheme.h"
#include "TranslationGizmo.h"

class CWorldEditor : public Palleon::CApplication
{
public:
	typedef std::map<uint32, UmbralActorPtr> ActorMap;

							CWorldEditor(bool);
	virtual					~CWorldEditor();

	bool					GetIsEmbedding() const;
	TouchFreeCameraPtr		GetMainCamera() const;
	TranslationGizmoPtr		GetTranslationGizmo() const;
	Palleon::SceneNodePtr	GetActorRotationNode() const;
	UmbralActorPtr			GetActor(uint32) const;
	const ActorMap&			GetActors() const;

	virtual void			Update(float) override;

	virtual void			NotifySizeChanged() override;

	virtual void			NotifyMouseMove(int, int) override;
	virtual void			NotifyMouseDown() override;
	virtual void			NotifyMouseUp() override;
	virtual void			NotifyMouseWheel(int) override;

	virtual void			NotifyKeyDown(Palleon::KEY_CODE) override;
	virtual void			NotifyKeyUp(Palleon::KEY_CODE) override;

	virtual void			NotifyInputCancelled() override;

	virtual std::string		NotifyExternalCommand(const std::string&) override;

private:
	typedef Palleon::CEmbedRemoteCall (CWorldEditor::*RpcMethodHandler)(const Palleon::CEmbedRemoteCall&);
	typedef std::map<std::string, RpcMethodHandler> RpcMethodHandlerMap;

	void						CreateWorld();
	void						CreateActor(uint32);
	void						SetActorPosition(uint32, const CVector3&);
	void						SetActorBaseModelId(uint32, uint32);
	void						SetActorTopModelId(uint32, uint32);
	void						CreateMap(uint32);

	Palleon::CEmbedRemoteCall	ProcessSetGamePath(const Palleon::CEmbedRemoteCall&);
	Palleon::CEmbedRemoteCall	ProcessSetMap(const Palleon::CEmbedRemoteCall&);
	Palleon::CEmbedRemoteCall	ProcessSetControlScheme(const Palleon::CEmbedRemoteCall&);
	Palleon::CEmbedRemoteCall	ProcessGetCameraPosition(const Palleon::CEmbedRemoteCall&);
	Palleon::CEmbedRemoteCall	ProcessSetCameraPosition(const Palleon::CEmbedRemoteCall&);
	Palleon::CEmbedRemoteCall	ProcessCreateActor(const Palleon::CEmbedRemoteCall&);
	Palleon::CEmbedRemoteCall	ProcessSetActorBaseModelId(const Palleon::CEmbedRemoteCall&);
	Palleon::CEmbedRemoteCall	ProcessSetActorTopModelId(const Palleon::CEmbedRemoteCall&);
	Palleon::CEmbedRemoteCall	ProcessSetActorPosition(const Palleon::CEmbedRemoteCall&);

	static const RpcMethodHandlerMap	g_rpcMethodHandlers;

	bool						m_isEmbedding = false;

	Palleon::ViewportPtr		m_mainViewport;
	Palleon::ViewportPtr		m_overlayViewport;
	TouchFreeCameraPtr			m_mainCamera;
	
	TranslationGizmoPtr			m_translationGizmo;
	Palleon::SceneNodePtr		m_actorRotationNode;
	UmbralMapPtr				m_map;
	ActorMap					m_actors;

	DebugOverlayPtr				m_debugOverlay;
	ControlSchemePtr			m_controlScheme;
};
