#pragma once

#include "Stream.h"

class CSectionLoader
{
public:
				CSectionLoader();
	virtual		~CSectionLoader();

	void		ReadSections(Framework::CStream&);

private:
	void		ReadSEDBSection(Framework::CStream&);
	void		ReadSEDBRESSection(Framework::CStream&);
};

