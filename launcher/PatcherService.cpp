#include "PatcherService.h"
#include "PatchFile.h"
#include "StdStreamUtils.h"

CPatcherService::CPatcherService()
{

}

CPatcherService::~CPatcherService()
{

}

void CPatcherService::Patch(const boost::filesystem::path& patchSrcPath, const boost::filesystem::path& patchDstPath, const ContinuationFunction& continuation)
{
	PATCHER_SERVICE_COMMAND command;
	command.patchDstPath = patchDstPath;
	command.patchSrcPath = patchSrcPath;
	LaunchCommand(command, continuation);
}

PATCHER_SERVICE_RESULT CPatcherService::Execute(const PATCHER_SERVICE_COMMAND& command)
{
	PATCHER_SERVICE_RESULT result;

	try
	{
		auto inputStream = Framework::CreateInputStdStream(command.patchSrcPath.native());
		auto patcherResult = CPatchFile::Execute(inputStream, command.patchDstPath);
		result.succeeded = patcherResult.succeeded;
	}
	catch(...)
	{

	}

	return result;
}
