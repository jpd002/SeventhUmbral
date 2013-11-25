#pragma once

#include <boost/filesystem.hpp>
#include "AsyncService.h"

struct PATCHER_SERVICE_COMMAND
{
	boost::filesystem::path			patchSrcPath;
	boost::filesystem::path			patchDstPath;
};

struct PATCHER_SERVICE_RESULT
{
	bool succeeded = false;
};

class CPatcherService : public CAsyncService<PATCHER_SERVICE_COMMAND, PATCHER_SERVICE_RESULT>
{
public:
										CPatcherService();
	virtual								~CPatcherService();
	
	void								Patch(const boost::filesystem::path&, const boost::filesystem::path&, const ContinuationFunction&);
	
protected:
	virtual PATCHER_SERVICE_RESULT		Execute(const PATCHER_SERVICE_COMMAND&) override;

private:
	
};
