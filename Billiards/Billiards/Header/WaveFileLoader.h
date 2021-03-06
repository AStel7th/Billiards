#pragma once
#include <Windows.h>
#include "func.h"
#include "Exception.h"

#define WAVEFILE_READ   1
#define WAVEFILE_WRITE  2

class WaveFileLoader
{
public:
	WAVEFORMATEX* m_pwfx;        // Pointer to WAVEFORMATEX structure
	HMMIO m_hmmio;       // MM I/O handle for the WAVE
	MMCKINFO m_ck;          // Multimedia RIFF chunk
	MMCKINFO m_ckRiff;      // Use in opening a WAVE file
	DWORD m_dwSize;      // The size of the wave file
	MMIOINFO m_mmioinfoOut;
	DWORD m_dwFlags;
	BOOL m_bIsReadingFromMemory;
	BYTE* m_pbData;
	BYTE* m_pbDataCur;
	ULONG m_ulDataSize;
	CHAR* m_pResourceBuffer;
	BYTE* pbWaveData;
protected:
	HRESULT ReadMMIO();
	HRESULT WriteMMIO(WAVEFORMATEX* pwfxDest);

public:
	WaveFileLoader();
	~WaveFileLoader();

	HRESULT Open(LPWSTR strFileName, WAVEFORMATEX* pwfx, DWORD dwFlags);
	HRESULT OpenFromMemory(BYTE* pbData, ULONG ulDataSize, WAVEFORMATEX* pwfx, DWORD dwFlags);
	HRESULT Close();

	HRESULT Read(BYTE* pBuffer, DWORD dwSizeToRead, DWORD* pdwSizeRead);
	HRESULT Write(UINT nSizeToWrite, BYTE* pbData, UINT* pnSizeWrote);

	DWORD   GetSize();
	HRESULT ResetFile();
	WAVEFORMATEX* GetFormat()
	{
		return m_pwfx;
	};

	BYTE* GetData()
	{
		return pbWaveData;
	}

};