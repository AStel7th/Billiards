// �C�����C���֐�
#pragma once
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
//#define _XM_NO_INTRINSICS_
#include <directxmath.h>

using namespace DirectX;
using namespace std;

// �f�o�b�O�p
#define _CRTDBG_MAP_ALLOC
#if defined(_DEBUG) && defined(_CRTDBG_MAP_ALLOC) && !defined(NEW)
#define NEW  ::new(_NORMAL_BLOCK, __FILE__, __LINE__)
#else
#define NEW  new
#endif

#define SAFE_RELEASE(x) if( x != nullptr ){ x->Release(); x = nullptr; }
#define SAFE_DELETE(x)  if( x != nullptr ){ delete x;  x = nullptr; }
#define SAFE_DELETE_ARRAY(x)  if( x != nullptr ){ delete[] x;  x = nullptr; }
#define SAFE_FREE(x)  if( x != nullptr ){ free( x );  x = nullptr; }

// �f�O���[�����W�A���ɕϊ�
inline double RADIAN(double degree)
{
	return degree * M_PI / 180.0;
}

// ���W�A�����f�O���[�ɕϊ�
inline double DEGREE(double radian)
{
	return radian * 180.0 / M_PI;
}

// �f�O���[�����W�A���ɕϊ��ӂ�[�Ƃ΂�
inline float RADIAN(float degree)
{
	return (float)(degree * M_PI / 180.0);
}

// ���W�A�����f�O���[�ɕϊ��ӂ�[�Ƃ΂�
inline float DEGREE(float radian)
{
	return (float)(radian * 180.0 / M_PI);
}

// ���b�Z�[�W�{�b�N�X��\�����܂��B
// windowname �E�B���h�E�̖��O�@�@val ���b�Z�[�W�{�b�N�X�̕���
inline void MSGBOX(LPCTSTR windowname, LPCTSTR val)
{
	MessageBox(nullptr, windowname, val, (MB_OK | MB_ICONWARNING));
}

template <typename T>
inline int ARRAY_NUM(T *&p){
	if (p){
		return sizeof(p) / sizeof(p[0]);
	}
	else
	{
		return 0;
	}
}

template <typename T>
inline int XOR_SWAP(T *a, T *b){
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}

inline DWORD XOR_RAND() {
	static DWORD x = 123456789; // ���@��
	static DWORD y = 362436069; // ���@��
	static DWORD z = 521288629; // �Ł@��
	static DWORD w = 88675123;  // ���@��
	DWORD t;

	t = x ^ (x << 11);
	x = y; y = z; z = w;
	return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

inline float XMVector3LengthSq(XMFLOAT3* f)
{
	float a;
	XMVECTOR vec = XMVector3LengthSq(XMLoadFloat3(f));
	XMStoreFloat(&a, vec);

	return a;
}