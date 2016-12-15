#include "../Header/AudioSystem.h"
#include "../Header/WaveFileLoader.h"

AudioSystem::AudioSystem() : pXAudio2(nullptr), pMasteringVoice(nullptr)
{

}

AudioSystem::~AudioSystem()
{
	// マスターボイス解放
	if (pMasteringVoice != nullptr) {
		pMasteringVoice->DestroyVoice();
		pMasteringVoice = nullptr;
	}

	// XAudio解放
	SAFE_RELEASE(pXAudio2);

	// COM解放
	CoUninitialize();
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



SoundPlayer::SoundPlayer() : soundData(nullptr), pSourceVoice(nullptr), pbWaveData(nullptr), buffer(nullptr)
{
}

SoundPlayer::~SoundPlayer()
{
	pSourceVoice->DestroyVoice();
	SAFE_DELETE_ARRAY(pbWaveData);
	SAFE_DELETE(buffer);
}

void SoundPlayer::Create(WaveFileLoader * pData,bool loop)
{
	soundData = pData;

	// Create the source voice
	AudioSystem::Instance().pXAudio2->CreateSourceVoice(&pSourceVoice, soundData->GetFormat());

	int loopCnt = 0;
	if (loop)
		loopCnt = XAUDIO2_LOOP_INFINITE;

	buffer = NEW XAUDIO2_BUFFER;
	buffer->Flags = XAUDIO2_END_OF_STREAM;
	buffer->AudioBytes = soundData->GetSize();
	buffer->pAudioData = soundData->GetData();
	buffer->PlayBegin = 0;
	buffer->PlayLength = 0;
	buffer->LoopBegin = 0;
	buffer->LoopLength = 0;
	buffer->LoopCount = loopCnt;
	pSourceVoice->SubmitSourceBuffer(buffer);
}

void SoundPlayer::Play()
{
	if (pSourceVoice == nullptr)
		return;

	Stop();
	pSourceVoice->FlushSourceBuffers();
	pSourceVoice->SubmitSourceBuffer(buffer);
	pSourceVoice->Start(0);
}

void SoundPlayer::Stop()
{
	if (pSourceVoice == nullptr)
		return;

	pSourceVoice->Stop(0);
}
