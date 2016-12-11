#pragma once
#include <windows.h>
#include <xaudio2.h>
#include "func.h"

class WaveFileLoader;

class AudioSystem
{
private:
	IXAudio2* pXAudio2;
	IXAudio2MasteringVoice* pMasteringVoice;
protected:
	AudioSystem();
public:
	virtual ~AudioSystem();

	static AudioSystem& Instance()
	{
		static AudioSystem inst;
		return inst;
	}

	HRESULT Create();

	void Play(WaveFileLoader* data);
};

class SoundPlayer
{
private:
	WaveFileLoader* soundData;
public:
	SoundPlayer();

	virtual ~SoundPlayer();

	void Create(WaveFileLoader* pData);

	void Play();
};