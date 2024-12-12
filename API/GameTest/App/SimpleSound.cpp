///////////////////////////////////////////////////////////////////////////////
// Filename: SimpleSound.cpp
// Provides a very simple miniaudio wrapper to load and play wav/mp3/flac files.
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
#include "stdafx.h"
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <assert.h>
//-----------------------------------------------------------------------------
#include "SimpleSound.h"
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Singleton Accessor.
//-----------------------------------------------------------------------------
CSimpleSound &CSimpleSound::GetInstance()
{
	static CSimpleSound theSoundClass;
	return theSoundClass;
}

CSimpleSound::CSimpleSound()
{
}

CSimpleSound::~CSimpleSound()
{
	if (m_initialized)
	{
		Shutdown();
	}
}

bool CSimpleSound::Initialize()
{
	ma_result result;

	result = ma_engine_init(NULL, &m_engine);

	assert(result == MA_SUCCESS && "Failed to initialize miniaudio library");

	m_initialized = result == MA_SUCCESS;

	return m_initialized;
}

void CSimpleSound::Shutdown()
{
	// Release the secondary buffers.
	for (auto& sound : m_sounds) 
	{
		ma_sound_uninit(&sound.second);
	}
	m_sounds.clear();

	ma_engine_uninit(&m_engine);

	m_initialized = false;
}

bool CSimpleSound::StartSound(const char *filename, const SoundFlags flags)
{
	if (m_sounds.find(filename) == m_sounds.end())
	{
		if (!LoadSound(filename))
		{
			return false;
		}
	}

	StopSound(filename);

	//Restarting sound back to beginning
	ma_sound_seek_to_pcm_frame(&m_sounds[filename], 0);
	ma_sound_set_looping(&m_sounds[filename], flags & SoundFlags::Looping);

	const ma_result result = ma_sound_start(&m_sounds[filename]);

	return result == MA_SUCCESS;
}


bool CSimpleSound::IsPlaying(const char *filename)
{
	auto it = m_sounds.find(filename);

	if (it == m_sounds.end())
	{
		return false;
	}

	return ma_sound_is_playing(&it->second);
}

bool CSimpleSound::StopSound(const char *filename)
{
	if (IsPlaying(filename))
	{
		const ma_result result = ma_sound_stop(&m_sounds[filename]);
		return result == MA_SUCCESS;
	}

	return false;
}

bool CSimpleSound::LoadSound(const char* filename)
{
	//Sound already found
	if (m_sounds.find(filename) != m_sounds.end())
	{
		return true;
	}

	//Create a sound object to manage
	ma_sound& sound = m_sounds[filename];

	const ma_result result = ma_sound_init_from_file(&m_engine, filename, 0, nullptr, nullptr, &sound);
	assert(result == MA_SUCCESS && "Unable to load sound file! Make sure file path is correct");
	if (result != MA_SUCCESS)
	{
		m_sounds.erase(filename);
		return false;
	}

	return true;
}