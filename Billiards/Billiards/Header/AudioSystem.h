#pragma once
#include <windows.h>
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
#include <xaudio2.h>
#else
#include <C:\Program Files (x86)\Microsoft DirectX SDK (June 2010)\Include\xaudio2.h>
#endif
#include "func.h"

class WaveFileLoader;

class AudioSystem
{
private:
	IXAudio2MasteringVoice* pMasteringVoice;
protected:
	AudioSystem();
public:
	IXAudio2* pXAudio2;

	virtual ~AudioSystem();

	static AudioSystem& Instance()
	{
		static AudioSystem inst;
		return inst;
	}

	HRESULT Create();
};

class SoundPlayer
{
private:
	IXAudio2SourceVoice* pSourceVoice;
	WaveFileLoader* soundData;
	XAUDIO2_BUFFER* buffer;
	BYTE* pbWaveData;
public:
	SoundPlayer();

	virtual ~SoundPlayer();

	void Create(WaveFileLoader* pData, bool loop = false);

	void Play();

	void Stop();
};