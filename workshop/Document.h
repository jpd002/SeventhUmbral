#pragma once

#include <string>
#include <boost/signals2.hpp>

class IDocument
{
public:
	typedef boost::signals2::signal<void ()> StateChangedEvent;

	virtual					~IDocument() {}

	virtual std::string		GetName() const = 0;

	virtual void			SetActive(bool) {}

	virtual bool			GetDirty() const { return false; }

	virtual void			Save() {}

	StateChangedEvent		StateChanged;
};
