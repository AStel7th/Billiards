#ifndef UDRAWFONT_H
#define UDRAWFONT_H

#include "Direct3D11.h"
#include "../HLSL/DebugFont_VS_Main.h"
#include "../HLSL/DebugFont_PS_Main.h"

class GraphicsPipeline;

class DebugFont
{
private:
	Direct3D11 &d3d11 = Direct3D11::Instance();

	// 頂点定義
	typedef struct _VERTEX
	{
		XMFLOAT3 pos;     // 頂点の座標
		XMFLOAT2 texel;   // テクセル座標
	}VERTEX;

	// 定数バッファの定義
	typedef struct _CONSTANT_BUFFER
	{
		XMFLOAT2 Position;    // スクリーン座標系上での表示位置
		XMFLOAT2 TexelOffset; // テクスチャー上のテクセル位置を指定するオフセット値
		XMFLOAT4 Color;       // 頂点カラー

		_CONSTANT_BUFFER(){};
		_CONSTANT_BUFFER(XMFLOAT2 position, XMFLOAT2 texelOffset, XMFLOAT4 color)
		{
			::CopyMemory(&Position, &position, sizeof(XMFLOAT2));
			::CopyMemory(&TexelOffset, &texelOffset, sizeof(XMFLOAT2));
			::CopyMemory(&Color, &color, sizeof(XMFLOAT4));
		};
	}CONSTANT_BUFFER;

	// 頂点バッファ
	ComPtr<ID3D11Buffer> m_pVertexBuffer;

	// シェーダーリソースビュー
	ComPtr<ID3D11ShaderResourceView> m_pSRView;

	// サンプラーステート
	ComPtr<ID3D11SamplerState> m_pSamplerState;

	// シェーダー用定数バッファ
	ComPtr<ID3D11Buffer> m_pConstantBuffers;

	GraphicsPipeline* m_pGraphicsPipeline;

	float m_FontWidth;   // ポリゴンの横幅
	float m_FontHeight;  // ポリゴンの縦幅
	static const int m_FontCnt = 95;     // テクスチャー上のフォント数

	TCHAR m_pStr[512];
	XMFLOAT2 m_Position;
	XMFLOAT4 m_Color;

public:
	DebugFont();
	virtual ~DebugFont();
	void Invalidate();

	// 作成処理
	// フォントサイズは、射影空間上での幅で指定する。1を指定するとスクリーンいっぱい分の大きさとなる
	// フォントサイズは作成時に設定した値を後で変更できない.
	void CreateMesh(float FontWidth, float FontHeight);

	// 文字は半角英数字と半角記号のみサポートする.それ以外の文字をパラメータに渡した場合の動作は保証しない.
	// 表示位置は射影空間上での座標で設定する
	void Update(TCHAR* pStr, XMFLOAT2* pPosition, XMFLOAT4* pColor);

	// デバック用のテキストを描画する
	void Draw();
};


// **********************************************************************************
// FPS描画クラス
// **********************************************************************************

class UFPS : private DebugFont
{
private:
	DWORD m_PrevTime;      // 前回時間
	WORD  m_FrameCounter;  // FPS合計数
	DWORD m_Frame;         // FPS計測値

public:
	UFPS();
	virtual ~UFPS();
	void Invalidate();
	void CreateMesh();
	void Update();
	void Draw();
};


//==================================================================================
//デバッグフォント描画クラス
//==================================================================================

class DebugText : private DebugFont
{
private:

public:
	DebugText();
	virtual ~DebugText();
	void Invalidate();
	void CreateMesh();
	void SetText(TCHAR* tchar, XMFLOAT2 *pos);
	void Draw();
};
#endif