#pragma once
#include <Windows.h>

//=======================================================
//�t���[�����[�g�𐧌䂷��N���X�ł��B
//======================================================
class TimeControl
{
public:
	TimeControl(DWORD dwFps = 60, bool bFrameSkip = true);
	~TimeControl();
private:
	DWORD m_dwLastFlipped;
	DWORD m_dwLastMinitues;
	DWORD m_dwFrameRate;
	DWORD m_dwSkipRate;
	float m_fFrameTime;
	bool m_bDrawFlag;
	bool m_bFrameSkip;
		
	bool m_bInit;
	//=======================================================
	//���݂̂P�b�P�ʂ̃��[�g�̕\��
	//======================================================
	DWORD m_dwFrameCount;
	DWORD m_dwSkipCount;
public:
	void Measure();
	void SetFPS(DWORD time);
	void OnFrameSkip(bool bFrameSkip);
	bool GetDrawFlag();
	void TimeRegular();
	DWORD GetSkipRate();
	DWORD GetFrameRate();
};