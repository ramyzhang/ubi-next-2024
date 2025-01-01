//-----------------------------------------------------------------------------
// SimpleController.h
// Provides a simple way to query input from keyboard+mouse and/or controllers via XINPUT.
//-----------------------------------------------------------------------------
#ifndef _SIMPLECONTROLLER_H
#define _SIMPLECONTROLLER_H

#if (_WIN32_WINNT >= 0x0604 /*_WIN32_WINNT_WIN8*/)
#include <XInput.h>
#pragma comment(lib,"xinput.lib")
#else
#include <XInput.h>
#pragma comment(lib,"xinput9_1_0.lib")
#endif

#define MAX_CONTROLLERS 4  // XInput handles up to 4 controllers 
#define THUMB_STICK_MAX_RANGE 32768.0f
#define TRIGGER_MAX_RANGE 255.0f

//-----------------------------------------------------------------------------
// CController
//-----------------------------------------------------------------------------

class CController
{
public:
	friend class CSimpleControllers;
	CController()
	{
		ZeroMemory(&m_state, sizeof(XINPUT_STATE));
	}	

	bool CheckButton(const int button, const bool onPress = true) const
	{
		if (onPress)
		{
			return (m_debouncedButtons & button) != 0;
		}
		else
		{
			return (m_lastButtons & button) != 0;
		}
	}

	float GetLeftThumbStickX() const
	{
		return (float)m_state.Gamepad.sThumbLX / THUMB_STICK_MAX_RANGE;
	}
	float GetLeftThumbStickY(const int pad = 0) const
	{
		return (float)m_state.Gamepad.sThumbLY / THUMB_STICK_MAX_RANGE;
	}
	float GetRightThumbStickX(const int pad = 0) const
	{
		return (float)m_state.Gamepad.sThumbRX / THUMB_STICK_MAX_RANGE;
	}
	float GetRightThumbStickY(const int pad = 0) const
	{
		return (float)m_state.Gamepad.sThumbRY / THUMB_STICK_MAX_RANGE;
	}
	float GetLeftTrigger(const int pad = 0) const
	{
		return (float)m_state.Gamepad.bLeftTrigger / TRIGGER_MAX_RANGE;
	}
	float GetRightTrigger(const int pad = 0) const
	{
		return (float)m_state.Gamepad.bRightTrigger / TRIGGER_MAX_RANGE;
	}
protected:
	XINPUT_STATE m_state;
	WORD m_lastButtons = 0;
	WORD m_debouncedButtons = 0;
	bool m_bConnected = false;
};

//-----------------------------------------------------------------------------
// CSimpleControllers
//-----------------------------------------------------------------------------

class CSimpleControllers
{
public:
	static CSimpleControllers &GetInstance();
	void Update();
	const CController &GetController(const int pad = 0)
	{
		const int padNum = (pad >= MAX_CONTROLLERS) ? 0 : pad;
		return m_Controllers[padNum];
	}
private:
	CController m_Controllers[MAX_CONTROLLERS];
};
#endif