#include "../Header/AudioSystem.h"
#include "../Header/WaveFileLoader.h"

AudioSystem::AudioSystem() : pXAudio2(nullptr), pMasteringVoice(nullptr)
{

}

AudioSystem::~AudioSystem()
{
}

HRESULT AudioSystem::Create()
{
	HRESULT hr;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	UINT32 flags = 0;

	if (FAILED(hr = XAudio2Create(&pXAudio2, flags)))
	{
		CoUninitialize();
		return hr;
	}

	if (FAILED(hr = pXAudio2->CreateMasteringVoice(&pMasteringVoice)))
	{
		SAFE_RELEASE(pXAudio2);
		CoUninitialize();
		return hr;
	}
}

void AudioSystem::Play(WaveFileLoader * data)
{
	HRESULT hr = S_OK;

	// Get format of wave file
	WAVEFORMATEX* pwfx = data->GetFormat();

	// Calculate how many bytes and samples are in the wave
	DWORD cbWaveSize = data->GetSize();

	// Read the sample data into memory
	BYTE* pbWaveData = NEW BYTE[cbWaveSize];

	if (FAILED(hr = data->Read(pbWaveData, cbWaveSize, &cbWaveSize)))
	{
		wprintf(L"Failed to read WAV data: %#X\n", hr);
		SAFE_DELETE_ARRAY(pbWaveData);
		return;
	}

	//
	// Play the wave using a XAudio2SourceVoice
	//

	// Create the source voice
	IXAudio2SourceVoice* pSourceVoice;
	if (FAILED(hr = pXAudio2->CreateSourceVoice(&pSourceVoice, pwfx)))
	{
		wprintf(L"Error %#X creating source voice\n", hr);
		SAFE_DELETE_ARRAY(pbWaveData);
		return;
	}

	// Submit the wave sample data using an XAUDIO2_BUFFER structure
	XAUDIO2_BUFFER buffer = { 0 };
	buffer.pAudioData = pbWaveData;
	buffer.Flags = XAUDIO2_END_OF_STREAM;  // tell the source voice not to expect any data after this buffer
	buffer.AudioBytes = cbWaveSize;

	if (FAILED(hr = pSourceVoice->SubmitSourceBuffer(&buffer)))
	{
		wprintf(L"Error %#X submitting source buffer\n", hr);
		pSourceVoice->DestroyVoice();
		SAFE_DELETE_ARRAY(pbWaveData);
		return;
	}

	hr = pSourceVoice->Start(0);

	// Let the sound play
	BOOL isRunning = TRUE;
	while (SUCCEEDED(hr) && isRunning)
	{
		XAUDIO2_VOICE_STATE state;
		pSourceVoice->GetState(&state);
		isRunning = (state.BuffersQueued > 0) != 0;

		// Wait till the escape key is pressed
		if (GetAsyncKeyState(VK_ESCAPE))
			break;

		Sleep(10);
	}

	// Wait till the escape key is released
	while (GetAsyncKeyState(VK_ESCAPE))
		Sleep(10);

	pSourceVoice->DestroyVoice();
	SAFE_DELETE_ARRAY(pbWaveData);
}



SoundPlayer::SoundPlayer() : soundData(nullptr)
{
}

SoundPlayer::~SoundPlayer()
{
}

void SoundPlayer::Create(WaveFileLoader * pData)
{
	soundData = pData;
}

void SoundPlayer::Play()
{
	AudioSystem::Instance().Play(soundData);
}
