#pragma once
#include <windows.h>
#include <tchar.h>

class UException
{
public:
	TCHAR m_pErrorStr[1024];

	UException(HRESULT hr, TCHAR* pMsg)
	{
		_stprintf_s(m_pErrorStr, _T("������HRESULT�F0x%x [ %s ]\n"), hr, pMsg);

#if defined(DEBUG) || defined(_DEBUG)
		// �G���[���f�o�b�K�ɏo��
		OutputDebugString(m_pErrorStr);
#endif
	}
};