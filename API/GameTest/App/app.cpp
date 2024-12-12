///////////////////////////////////////////////////////////////////////////////
// Filename: App.cpp
// Provides a number of basic helper functions that wrap around thing like rendering, sound, and input handling
///////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------
#include "stdafx.h"
//---------------------------------------------------------------------------------
#include <string>
#include "main.h"
#include "app.h"
#include "SimpleSound.h"
#include "SimpleController.h"
#include "SimpleSprite.h"

//---------------------------------------------------------------------------------
// Utils and externals for system info.

namespace App
{	
	void DrawLine(const float sx, const float sy, const float ex, const float ey, const float r, const float g, const float b)
	{
		float startX = sx;
		float startY = sy;
		float endX = ex;
		float endY = ey;
#if APP_USE_VIRTUAL_RES		
		APP_VIRTUAL_TO_NATIVE_COORDS(startX, startY);
		APP_VIRTUAL_TO_NATIVE_COORDS(endX, endY);
#endif
		glBegin(GL_LINES);
		glColor3f(r, g, b); // Yellow
		glVertex2f(startX, startY);
		glVertex2f(endX, endY);
		glEnd();
	}
	
	CSimpleSprite *CreateSprite(const char *fileName, const int columns, const int rows)
	{
		return new CSimpleSprite(fileName, columns, rows);
	}

	bool IsKeyPressed(const int key)
	{
		return ((GetAsyncKeyState(key) & 0x8000) != 0);
	}

	void GetMousePos(float &x, float &y)
	{
		POINT mousePos;
		GetCursorPos(&mousePos);	// Get the mouse cursor 2D x,y position			
		ScreenToClient(MAIN_WINDOW_HANDLE, &mousePos);
		x = (float)mousePos.x;
		y = (float)mousePos.y;
		x = (x * (2.0f / WINDOW_WIDTH) - 1.0f);
		y = -(y * (2.0f / WINDOW_HEIGHT) - 1.0f);

#if APP_USE_VIRTUAL_RES		
		APP_NATIVE_TO_VIRTUAL_COORDS(x, y);
#endif
	}

	void PlaySound(const char *fileName, const bool looping)
	{
		const SoundFlags flags = (looping) ? SoundFlags::Looping : SoundFlags::None;
		CSimpleSound::GetInstance().StartSound(fileName, flags);
	}

	void StopSound(const char *fileName)
	{
		CSimpleSound::GetInstance().StopSound(fileName);
	}

	bool IsSoundPlaying(const char *fileName)
	{
		return CSimpleSound::GetInstance().IsPlaying(fileName);
	}

	// This prints a string to the screen
	void Print(const float x, const float y, const char *st, const float r, const float g, const float b, void *font)
	{
		float xPos = x;
		float yPos = y;
#if APP_USE_VIRTUAL_RES		
		APP_VIRTUAL_TO_NATIVE_COORDS(xPos, yPos);
#endif		
		// Set location to start printing text
		glColor3f(r, g, b); // Yellow
		glRasterPos2f(xPos, yPos);
		int l = (int)strlen(st);
		for (int i = 0; i < l; i++)
		{
			glutBitmapCharacter(font, st[i]); // Print a character on the screen
		}
	}

	const CController &GetController(const int pad )
	{
		return CSimpleControllers::GetInstance().GetController(pad);
	}
}