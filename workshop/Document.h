#pragma once

#include <string>

class IDocument
{
public:
	virtual					~IDocument() {}

	virtual std::string		GetName() const = 0;
	
	virtual void			SetActive(bool) {}
};
