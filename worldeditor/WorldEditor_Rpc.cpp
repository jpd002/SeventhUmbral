#include <boost/lexical_cast.hpp>
#include "WorldEditor.h"
#include "RpcDefs.h"
#include "math/MathStringUtils.h"
#include "../dataobjects/FileManager.h"
#include "EditorControlScheme.h"
#include "ViewerControlScheme.h"

const CWorldEditor::RpcMethodHandlerMap CWorldEditor::g_rpcMethodHandlers = 
{
	std::make_pair(WORLDEDITOR_METHOD_SETGAMEPATH,			&CWorldEditor::ProcessSetGamePath),
	std::make_pair(WORLDEDITOR_METHOD_SETMAP,				&CWorldEditor::ProcessSetMap),
	std::make_pair(WORLDEDITOR_METHOD_CREATEACTOR,			&CWorldEditor::ProcessCreateActor),
	std::make_pair(WORLDEDITOR_METHOD_SETACTORBASEMODELID,	&CWorldEditor::ProcessSetActorBaseModelId),
	std::make_pair(WORLDEDITOR_METHOD_SETACTORTOPMODELID,	&CWorldEditor::ProcessSetActorTopModelId),
	std::make_pair(WORLDEDITOR_METHOD_SETACTORPOSITION,		&CWorldEditor::ProcessSetActorPosition),
	std::make_pair(WORLDEDITOR_METHOD_SETCONTROLSCHEME,		&CWorldEditor::ProcessSetControlScheme),
	std::make_pair(WORLDEDITOR_METHOD_GETCAMERAPOSITION,	&CWorldEditor::ProcessGetCameraPosition),
	std::make_pair(WORLDEDITOR_METHOD_SETCAMERAPOSITION,	&CWorldEditor::ProcessSetCameraPosition)
};

static Palleon::CEmbedRemoteCall MakeSuccessRpc()
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetParam(WORLDEDITOR_PARAM_RESULT, WORLDEDITOR_RESULT_SUCCEEDED);
	return rpc;
}

static Palleon::CEmbedRemoteCall MakeFailureRpc()
{
	Palleon::CEmbedRemoteCall rpc;
	rpc.SetParam(WORLDEDITOR_PARAM_RESULT, WORLDEDITOR_RESULT_FAILED);
	return rpc;
}

std::string CWorldEditor::NotifyExternalCommand(const std::string& command)
{
	Palleon::CEmbedRemoteCall inRpc(command);
	auto method = inRpc.GetMethod();
	auto methodHandlerIterator = g_rpcMethodHandlers.find(method);
	assert(methodHandlerIterator != std::end(g_rpcMethodHandlers));
	if(methodHandlerIterator == std::end(g_rpcMethodHandlers))
	{
		return MakeFailureRpc().ToString();
	}
	auto outRpc = ((this)->*(methodHandlerIterator->second))(inRpc);
	return outRpc.ToString();
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessSetGamePath(const Palleon::CEmbedRemoteCall& rpc)
{
	auto gamePath = rpc.GetParam(WORLDEDITOR_PARAM_PATH);
	CFileManager::SetGamePath(gamePath);
	return MakeSuccessRpc();
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessSetMap(const Palleon::CEmbedRemoteCall& rpc)
{
	try
	{
		auto mapId = boost::lexical_cast<uint32>(rpc.GetParam(WORLDEDITOR_PARAM_MAPID));
		CreateMap(mapId);
		return MakeSuccessRpc();
	}
	catch(...)
	{
		return MakeFailureRpc();
	}
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessSetControlScheme(const Palleon::CEmbedRemoteCall& rpc)
{
	try
	{
		Palleon::CEmbedRemoteCall outRpc;
		auto controlSchemeId = boost::lexical_cast<uint32>(rpc.GetParam(WORLDEDITOR_PARAM_CONTROLSCHEME));
		switch(controlSchemeId)
		{
		case WORLDEDITOR_CONTROLSCHEME_EDITOR:
			m_controlScheme = std::make_shared<CEditorControlScheme>(*this);
			break;
		case WORLDEDITOR_CONTROLSCHEME_VIEWER:
			m_controlScheme = std::make_shared<CViewerControlScheme>(*this);
			break;
		default:
			throw std::runtime_error("Invalid control scheme.");
			break;
		}
		outRpc.SetParam(WORLDEDITOR_PARAM_RESULT, WORLDEDITOR_RESULT_SUCCEEDED);
		return outRpc.ToString();
	}
	catch(...)
	{
		return MakeFailureRpc();
	}
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessGetCameraPosition(const Palleon::CEmbedRemoteCall& rpc)
{
	try
	{
		Palleon::CEmbedRemoteCall outRpc;
		outRpc.SetParam(WORLDEDITOR_PARAM_RESULT, WORLDEDITOR_RESULT_SUCCEEDED);
		outRpc.SetParam(WORLDEDITOR_PARAM_POSITION, MathStringUtils::ToString(m_mainCamera->GetPosition()));
		return outRpc.ToString();
	}
	catch(...)
	{
		return MakeFailureRpc();
	}
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessSetCameraPosition(const Palleon::CEmbedRemoteCall& rpc)
{
	try
	{
		auto position = MathStringUtils::ParseVector3(rpc.GetParam(WORLDEDITOR_PARAM_POSITION));
		m_mainCamera->SetPosition(position);
		return MakeSuccessRpc();
	}
	catch(...)
	{
		return MakeFailureRpc();
	}
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessCreateActor(const Palleon::CEmbedRemoteCall& rpc)
{
	try
	{
		auto id = boost::lexical_cast<uint32>(rpc.GetParam(WORLDEDITOR_PARAM_ID));
		CreateActor(id);
		return MakeSuccessRpc();
	}
	catch(...)
	{
		return MakeFailureRpc();
	}
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessSetActorBaseModelId(const Palleon::CEmbedRemoteCall& rpc)
{
	try
	{
		auto actorId = boost::lexical_cast<uint32>(rpc.GetParam(WORLDEDITOR_PARAM_ID));
		auto baseModelId = boost::lexical_cast<uint32>(rpc.GetParam(WORLDEDITOR_PARAM_BASEMODELID));
		SetActorBaseModelId(actorId, baseModelId);
		return MakeSuccessRpc();
	}
	catch(...)
	{
		return MakeFailureRpc();
	}
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessSetActorTopModelId(const Palleon::CEmbedRemoteCall& rpc)
{
	try
	{
		auto actorId = boost::lexical_cast<uint32>(rpc.GetParam(WORLDEDITOR_PARAM_ID));
		auto topModelId = boost::lexical_cast<uint32>(rpc.GetParam(WORLDEDITOR_PARAM_TOPMODELID));
		SetActorTopModelId(actorId, topModelId);
		return MakeSuccessRpc();
	}
	catch(...)
	{
		return MakeFailureRpc();
	}
}

Palleon::CEmbedRemoteCall CWorldEditor::ProcessSetActorPosition(const Palleon::CEmbedRemoteCall& rpc)
{
	try
	{
		auto id = boost::lexical_cast<uint32>(rpc.GetParam(WORLDEDITOR_PARAM_ID));
		auto position = MathStringUtils::ParseVector3(rpc.GetParam(WORLDEDITOR_PARAM_POSITION));
		auto actorIterator = m_actors.find(id);
		if(actorIterator == std::end(m_actors))
		{
			return MakeFailureRpc();
		}
		auto& actor = actorIterator->second;
		actor->SetPosition(position);
		return MakeSuccessRpc();
	}
	catch(...)
	{
		return MakeFailureRpc();
	}
}
