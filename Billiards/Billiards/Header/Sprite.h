#pragma once
#include "Direct3D11.h"
#include <map>
#include <list>
#include <locale.h>
#include "Shader.h"

//====================================================================
//  スプライト関連
//====================================================================

// テクスチャの情報
class TextureInfo 
{
public:
	TexMetadata metadata;
	ScratchImage image;    // テクスチャへのポインタ
	XMFLOAT4 uvrect;            // uv記憶

	TextureInfo();
	TextureInfo(const TextureInfo& r);
	virtual ~TextureInfo();

	TextureInfo &operator =(const TextureInfo &r);

	void AddUV(int texNum, int left, int top, int width, int height, int uvid);// UV設定するお texNum:設定するテクスチャID left:基点のx座標 top:基点のy座標 width:幅 height:高さ uvid:登録するuv番号
};

// テクスチャまとめたクラス
class TextureContainer {
	map<string, TextureInfo> data; // 読み込んだデータ
protected:
	TextureContainer();
public:

	// シングルトンオブジェクトを取得
	static TextureContainer &Instance()
	{
		static TextureContainer inst;
		return inst;
	}

	virtual ~TextureContainer();
	TextureInfo LoadTexture(const string& t_pTextureName);   // テクスチャを読み込みます。 t_pTextureName：ファイル名　texNum:識別ID

private:

};

class Sprite
{
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

		_CONSTANT_BUFFER() {};
		_CONSTANT_BUFFER(XMFLOAT2 position, XMFLOAT2 texelOffset)
		{
			::CopyMemory(&Position, &position, sizeof(XMFLOAT2));
			::CopyMemory(&TexelOffset, &texelOffset, sizeof(XMFLOAT2));
		};
	}CONSTANT_BUFFER;

	// 定数バッファの定義
	typedef struct _OBJ_BUFFER
	{
		XMFLOAT4X4 World;
		XMFLOAT4X4 Proj;
		FLOAT Uv_left;
		FLOAT Uv_top;
		FLOAT Uv_width;
		FLOAT Uv_height;
		XMFLOAT4 Color;

	}OBJ_BUFFER;

	Direct3D11 &d3d11 = Direct3D11::Instance();

	// 頂点バッファ
	static ComPtr<ID3D11Buffer> m_pVertexBuffer;

	static list<Sprite*> drawObjectList;		// 描画対象リスト

												// シェーダー用定数バッファ
	static ComPtr<ID3D11Buffer> m_pObjBuffers;

	//インプットレイアウト
	static ComPtr<ID3D11InputLayout> pInputLayout;

	// シェーダーリソースビュー
	ComPtr<ID3D11ShaderResourceView> m_pSRView;

	// サンプラーステート
	static ComPtr<ID3D11SamplerState> m_pSamplerState;

	static unique_ptr<VertexShader> pVertexShader;
	static unique_ptr<PixelShader> pPixelShader;

	TextureInfo* tex;		// テクスチャ
	shared_ptr<TextureInfo> subtex;		// テクスチャ

	int scW, scH;			// スクリーンサイズ
	int polyW, polyH;		// 板ポリゴンサイズ
	float pivotX, pivotY;	// ピボット座標
	float posX, posY, posZ;	// 位置
	float rad;				// 回転角度（ラジアン）
	float scaleX, scaleY;	// スケール
	float uvLeft, uvTop;	// UV左上座標
	float uvW, uvH;			// UV幅高
	XMFLOAT4 color;
	float SubuvLeft, SubuvTop;	// subUV左上座標
	float SubuvW, SubuvH;			// subUV幅高

									// コピー
	void copy(const Sprite &r);

public:
	Sprite();
	Sprite(int screenWidth, int screenHeight); // スクリーンサイズを指定します
	Sprite(const Sprite &r);
	~Sprite();



	Sprite &operator =(const Sprite &r) { copy(r); return *this; }

	// 静的領域生成
	static void Create();

	// 終了
	static void end_last();

	// テクスチャ設定
	void SetTexture(TextureInfo* tex);

	void SetSubTexture(TextureInfo* tex);

	// 板ポリサイズ指定
	void SetSize(int w, int h);
	void GetSize(int* w, int* h);
	XMINT2 GetSize();

	// スクリーンサイズ指定
	void SetScreenSize(int w, int h);

	// ピボット指定
	void SetPivot(float x, float y);

	// 姿勢指定
	void SetPos(float x, float y);
	void GetPos(float* x, float* y);
	XMFLOAT2* GetPos() { return &XMFLOAT2(posX, posY); };
	void SetRotate(float deg);
	float GetRotate();
	void SetScale(float sx, float sy);

	// UV切り取り指定
	void SetUV(float l, float t, float w, float h);
	// UV切り取り指定
	void SetSubUV(float l, float t, float w, float h);

	// α設定
	void SetColor(XMFLOAT4& a);
	XMFLOAT4& GetColor();

	// プライオリティ設定
	void SetPriority(float z);
	float GetPriority();

	// 描画リストに追加
	void Draw();

	// 描画リストを描画
	static void DrawAll();

	// 描画リストクリア
	static void Sprite::ClearDrawList();

	// 簡単初期化
	void Init(TextureInfo* tex, float x = 0.0f, float y = 0.0f);
	// 簡単初期化 中心点を画像の中央に指定
	void InitCenter(TextureInfo* tex, float x = 0.0f, float y = 0.0f);
	// テクスチャに設定したuvをセットします
	void SetUVFromTex(int uvid, float x = 0.0f, float y = 0.0f);
	void SetUVFromTexCenter(int uvid);
};