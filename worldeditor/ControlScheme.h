#pragma once

class CControlScheme
{
public:
	virtual			~CControlScheme() {}

	virtual void	PreUpdate(float) {}
	virtual void	PostUpdate(float) {}
	
	virtual void	NotifyActorChanged(uint32) {}

	virtual void	NotifyMouseMove(int, int) {}
	virtual void	NotifyMouseDown() {}
	virtual void	NotifyMouseUp() {}
	virtual void	NotifyMouseWheel(int) {}

	virtual void	NotifyKeyDown(Palleon::KEY_CODE) {}
	virtual void	NotifyKeyUp(Palleon::KEY_CODE) {}
};

typedef std::shared_ptr<CControlScheme> ControlSchemePtr;
