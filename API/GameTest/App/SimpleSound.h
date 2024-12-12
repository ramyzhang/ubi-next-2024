//-----------------------------------------------------------------------------
// SimpleSound.h
// Provides a very simple miniaudio wrapper to load and play wav/mp3/flac files.
//-----------------------------------------------------------------------------
#ifndef _SIMPLESOUND_H_
#define _SIMPLESOUND_H_

#include "miniaudio/miniaudio.h"

#include <map>

//-----------------------------------------------------------------------------
// CSimpleSound
//-----------------------------------------------------------------------------

enum SoundFlags
{
	None = 0
	, Looping = 1 << 1
};

class CSimpleSound
{
public:

	static CSimpleSound &CSimpleSound::GetInstance();
	CSimpleSound();	
	~CSimpleSound();

	bool Initialize();
	void Shutdown();
	
	bool StartSound(const char *filename, const SoundFlags flags = SoundFlags::None);
	bool StopSound(const char *filename);
	bool IsPlaying(const char *filename);
	
private:
	bool LoadSound(const char* filename);

	ma_engine m_engine;
	std::map<const char*, ma_sound> m_sounds;
	bool m_initialized = false;
};

#endif