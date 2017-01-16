#pragma once

#include "win32/Dialog.h"
#include "win32/Edit.h"
#include "win32/PropertyGrid.h"
#include <boost/signals2.hpp>

class CZoneEditorPropertiesPane : public Framework::Win32::CDialog
{
public:
								CZoneEditorPropertiesPane(HWND);
	virtual						~CZoneEditorPropertiesPane();

	void						SetSelection(const Framework::Win32::PropertyBagPtr&);

	void						Refresh();

protected:
	long						OnSize(unsigned int, unsigned int, unsigned int) override;

private:
	typedef std::unique_ptr<Framework::Win32::CPropertyGrid> PropertyGridPtr;

	PropertyGridPtr				m_propertyGrid;
};
