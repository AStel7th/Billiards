#pragma once
#include <Windows.h>
#include <DirectXMath.h>
#include <vector>
#include <list>
#include "func.h"

#define TREEMANAGER_MAXLEVEL		7

using namespace std;
using namespace DirectX;

class Cell;
class Collider;

// 分木クラス
class ObjectTree
{
public:
	Cell*		pCell;		// 登録空間
	Collider*	pCol;		// 判定対象コライダー
	ObjectTree* pPre;		// 前のObjectTreeへのポインタ
	ObjectTree* pNext;		// 次のObjectTreeへのポインタ

public:
	ObjectTree();

	virtual ~ObjectTree() {}

public:
	// 自らリストから外れる
	bool Remove();

	void RegistCell(Cell* pC);

	ObjectTree* GetNextObj();
};


// 分木空間管理クラス
class SpaceDivision
{
protected:
	unsigned int	dim;
	Cell**			ppCellAry;						// 線形空間ポインタ配列
	unsigned int	iPow[TREEMANAGER_MAXLEVEL + 1];	// べき乗数値配列
	XMFLOAT3		width;							// 領域の幅
	XMFLOAT3		regMin;							// 領域の最小値
	XMFLOAT3		regMax;							// 領域の最大値
	XMFLOAT3		unit;							// 最小領域の辺の長さ
	DWORD			cellNum;						// 空間の数
	unsigned int	level;							// 最下位レベル

	SpaceDivision();

public:

	// シングルトンオブジェクトを取得
	static SpaceDivision &Instance()
	{
		static SpaceDivision inst;
		return inst;
	}

	virtual ~SpaceDivision();

	bool Init(unsigned int Level, XMFLOAT3 &Min, XMFLOAT3 &Max);

	bool Regist(XMFLOAT3 *Min, XMFLOAT3 *Max, ObjectTree* pOT);

	DWORD GetAllCollisionList(vector<Collider*> &ColVect);

protected:
	// 空間内で衝突リストを作成する
	bool GetCollisionList(DWORD Elem, vector<Collider*> &ColVect, list<Collider*> &ColStac);

	// 空間を生成
	bool CreateNewCell(DWORD Elem);

	// 座標から空間番号を算出
	DWORD GetMortonNumber(XMFLOAT3 *Min, XMFLOAT3 *Max);

	// ビット分割関数
	DWORD BitSeparateFor3D(BYTE n);

	// 3Dモートン空間番号算出関数
	DWORD Get3DMortonNumber(BYTE x, BYTE y, BYTE z);

	// 座標→線形8分木要素番号変換関数
	DWORD GetPointElem(XMFLOAT3 &p);
};



// 空間クラス
class Cell
{
protected:
	ObjectTree* pLatest;

public:

	Cell() : pLatest(nullptr){}

	virtual ~Cell();

	void ResetLink(ObjectTree* pOT);

	bool Push(ObjectTree* pOT);
	
	ObjectTree* GetFirstObj();

	bool OnRemove(ObjectTree* pRemoveObj);
};