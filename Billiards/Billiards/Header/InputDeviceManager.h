#pragma once
#include <windows.h>
#include <dinput.h>

class InputDeviceManager
{
protected:
	InputDeviceManager();
public:
	static InputDeviceManager& Instance()
	{
		static InputDeviceManager inst;
		return inst;
	}

	virtual ~InputDeviceManager();


};