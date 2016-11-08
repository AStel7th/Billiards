// インライン関数
#pragma once
#include <Windows.h>
#define _USE_MATH_DEFINES
#include <math.h>
//#define _XM_NO_INTRINSICS_
#include <directxmath.h>

using namespace DirectX;
using namespace std;

// デバッグ用
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

// デグリーをラジアンに変換
inline double RADIAN(double degree)
{
	return degree * M_PI / 180.0;
}

// ラジアンをデグリーに変換
inline double DEGREE(double radian)
{
	return radian * 180.0 / M_PI;
}

// デグリーをラジアンに変換ふろーとばん
inline float RADIAN(float degree)
{
	return (float)(degree * M_PI / 180.0);
}

// ラジアンをデグリーに変換ふろーとばｎ
inline float DEGREE(float radian)
{
	return (float)(radian * 180.0 / M_PI);
}

// メッセージボックスを表示します。
// windowname ウィンドウの名前　　val メッセージボックスの文章
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
	static DWORD x = 123456789; // い　な
	static DWORD y = 362436069; // い　ん
	static DWORD z = 521288629; // で　で
	static DWORD w = 88675123;  // す　も
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