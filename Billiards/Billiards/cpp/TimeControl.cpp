#include "../Header/TimeControl.h"

//======================================================
//コンストラクタ
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
//デストラクタ
//======================================================
TimeControl::~TimeControl()
{
	timeEndPeriod(1);
}

//======================================================
//ここで時間制御をします。
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
		//項の移動をしたことで無駄な演算を減らしました。
		//条件			timeGetTime() - m_dwLastMinitues <= (DWORD)((m_dwFrameCount + 1) * m_fFrameTime)
		//項の移動		timeGetTime()  <= (DWORD)((m_dwFrameCount + 1) * m_fFrameTime) +  m_dwLastMinitues
		//変数置き換え	timeGetTime()　<= dwTime
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
//最大フレームレートを測定します。
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
//フレームスキップをするかしないかを設定します。
//======================================================
void TimeControl::OnFrameSkip(bool bFrameSkip)
{
	m_bFrameSkip = bFrameSkip;
}

//======================================================
//ここでFPSを設定する。
//======================================================
void TimeControl::SetFPS(DWORD fps)
{
	m_fFrameTime = 1000.0f / fps;
}


//======================================================
//ここでフレームレートを取得します。
//======================================================
DWORD TimeControl::GetFrameRate()
{
	return m_dwFrameRate;
}

//======================================================
//ここでスキップレートを取得します。
//======================================================
DWORD TimeControl::GetSkipRate()
{
	return m_dwSkipRate;
}

//======================================================
//描画するかしないかを判定
//======================================================
bool TimeControl::GetDrawFlag()
{
	return m_bDrawFlag;
}