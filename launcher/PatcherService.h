#pragma once

#include <boost/filesystem.hpp>
#include "AsyncService.h"

enum PATCHER_SERVICE_RESULT
{
	PATCHER_SERVICE_RESULT_SUCCESS,
	PATCHER_SERVICE_RESULT_ERROR,
};

struct PATCHER_SERVICE_COMMAND
{
	boost::filesystem::path			patchSrcPath;
	boost::filesystem::path			patchDstPath;
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
