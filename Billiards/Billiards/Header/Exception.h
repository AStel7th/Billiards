#pragma once
#include <windows.h>
#include <tchar.h>

class UException
{
public:
	TCHAR m_pErrorStr[1024];

	UException(HRESULT hr, TCHAR* pMsg)
	{
		_stprintf_s(m_pErrorStr, _T("■□■HRESULT：0x%x [ %s ]\n"), hr, pMsg);

#if defined(DEBUG) || defined(_DEBUG)
		// エラーをデバッガに出力
		OutputDebugString(m_pErrorStr);
#endif
	}
};