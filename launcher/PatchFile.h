#pragma once

#include "Stream.h"
#include "StdStream.h"
#include <boost/filesystem.hpp>

struct PATCH_RESULT
{
	bool succeeded = false;
	std::vector<std::string> messages;
};

class CPatchFile
{
public:
	static PATCH_RESULT			Execute(Framework::CStream&, const boost::filesystem::path&);

private:
								CPatchFile(const boost::filesystem::path&);
	virtual						~CPatchFile();

	const PATCH_RESULT&			GetResult() const;

	void						DoExecute(Framework::CStream&);

	void						ExecuteFHDR(Framework::CStream&);
	void						ExecuteDIFF(Framework::CStream&);
	void						ExecuteHIST(Framework::CStream&);
	void						ExecuteAPLY(Framework::CStream&);
	void						ExecuteADIR(Framework::CStream&);
	void						ExecuteDELD(Framework::CStream&);
	void						ExecuteETRY(Framework::CStream&);

	Framework::CStdStream		CreateOutputStdStreamWithRetry(const boost::filesystem::path&);

	void						ExtractUncompressed(Framework::CStream&, Framework::CStream&, uint32);
	void						ExtractCompressed(Framework::CStream&, Framework::CStream&, uint32);

	boost::filesystem::path		m_gameLocationPath;
	PATCH_RESULT				m_result;
};
