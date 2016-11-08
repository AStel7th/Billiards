#include "../Header/TimeControl.h"

//======================================================
//�R���X�g���N�^
//======================================================
TimeControl::TimeControl(DWORD dwFps, bool bFrameSkip)
{
	memset(this,0,sizeof(TimeControl));
	m_bInit = 1;
	m_bFrameSkip = bFrameSkip;
	SetFPS(dwFps);
	timeBeginPeriod(1);
}


//======================================================
//�f�X�g���N�^
//======================================================
TimeControl::~TimeControl()
{
	timeEndPeriod(1);
}

//======================================================
//�����Ŏ��Ԑ�������܂��B
//======================================================
void TimeControl::TimeRegular()
{
	m_dwFrameCount++;

	
	if(m_bInit == 1)
	{
		m_dwLastMinitues = timeGetTime();
		m_bInit = 0;
		m_bDrawFlag = 1;
		return;
	}

	if(m_bFrameSkip == 1 && timeGetTime()  > (DWORD)((m_dwFrameCount + 1) * m_fFrameTime + m_dwLastMinitues))
	{
		m_bDrawFlag = 0;
		m_dwSkipCount++;
	}
	else
	{
		//===========================================================
		//���̈ړ����������ƂŖ��ʂȉ��Z�����炵�܂����B
		//����			timeGetTime() - m_dwLastMinitues <= (DWORD)((m_dwFrameCount + 1) * m_fFrameTime)
		//���̈ړ�		timeGetTime()  <= (DWORD)((m_dwFrameCount + 1) * m_fFrameTime) +  m_dwLastMinitues
		//�ϐ��u������	timeGetTime()�@<= dwTime
		//===========================================================
		DWORD dwTime = (DWORD)(m_dwFrameCount * m_fFrameTime + m_dwLastMinitues);
		while(timeGetTime() <= dwTime)
		{
			Sleep(1);
		}
		m_bDrawFlag = 1;
	}

	
	if(timeGetTime() - m_dwLastMinitues >= 1000)
	{
		m_dwLastMinitues = timeGetTime();
		m_dwFrameRate = m_dwFrameCount;
		m_dwFrameCount = 0;
		m_dwSkipRate = m_dwSkipCount;
		m_dwSkipCount = 0;
	}
}

//==================================================
//�ő�t���[�����[�g�𑪒肵�܂��B
//==================================================
void TimeControl::Measure()
{
	m_dwFrameCount++;
	m_bDrawFlag = 1;
	if(timeGetTime() - m_dwLastMinitues >= 1000)
	{
		m_dwLastMinitues = timeGetTime();
		m_dwFrameRate = m_dwFrameCount;
		m_dwFrameCount = 0;
		m_dwSkipRate = m_dwSkipCount;
		m_dwSkipCount = 0;
	}	
}

//======================================================
//�t���[���X�L�b�v�����邩���Ȃ�����ݒ肵�܂��B
//======================================================
void TimeControl::OnFrameSkip(bool bFrameSkip)
{
	m_bFrameSkip = bFrameSkip;
}

//======================================================
//������FPS��ݒ肷��B
//======================================================
void TimeControl::SetFPS(DWORD fps)
{
	m_fFrameTime = 1000.0f / fps;
}


//======================================================
//�����Ńt���[�����[�g���擾���܂��B
//======================================================
DWORD TimeControl::GetFrameRate()
{
	return m_dwFrameRate;
}

//======================================================
//�����ŃX�L�b�v���[�g���擾���܂��B
//======================================================
DWORD TimeControl::GetSkipRate()
{
	return m_dwSkipRate;
}

//======================================================
//�`�悷�邩���Ȃ����𔻒�
//======================================================
bool TimeControl::GetDrawFlag()
{
	return m_bDrawFlag;
}