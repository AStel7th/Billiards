#include "../Header/SpaceDivision.h"
#include "../Header/Layer.h"
#include "../Header/GameObject.h"
#include "../Header/Collider.h"

// class ObjectTree
ObjectTree::ObjectTree() : pCell(nullptr), pCol(nullptr), pPre(nullptr), pNext(nullptr)
{

}

bool ObjectTree::Remove()
{
	if (!pCell)
		return false;

	if (!pCell->OnRemove(this))
		return false;

	if (pPre != nullptr)
	{
		pPre->pNext = pNext;
		pPre = nullptr;
	}

	if (pNext != nullptr)
	{
		pNext->pPre = pPre;
		pNext = nullptr;
	}

	pCell = nullptr;
	return true;
}

void ObjectTree::RegistCell(Cell * pC)
{
	pCell = pC;
}

ObjectTree* ObjectTree::GetNextObj()
{
	return pNext;
}



// class SpaceDivision
SpaceDivision::SpaceDivision()
{
	dim = 0;
	ppCellAry = nullptr;
	width = XMFLOAT3(1.0f, 1.0f, 1.0f);
	regMin = XMFLOAT3(0.0f, 0.0f, 0.0f);
	regMax = XMFLOAT3(1.0f, 1.0f, 1.0f);
	unit = XMFLOAT3(1.0f, 1.0f, 1.0f);
	cellNum = 0;
	level = 0;
}

SpaceDivision::~SpaceDivision()
{
	DWORD i;

	for (i = 0; i < cellNum; i++)
	{
		if (ppCellAry[i] != nullptr)
			SAFE_DELETE(ppCellAry[i]);
	}
	SAFE_DELETE_ARRAY(ppCellAry);
}

bool SpaceDivision::Init(unsigned int Level, XMFLOAT3 & Min, XMFLOAT3 & Max)
{
	// 設定最高レベル以上の空間は作れない
	if (Level >= TREEMANAGER_MAXLEVEL)
		return false;

	// 各レベルでの空間数を算出
	int i;
	iPow[0] = 1;
	for (i = 1; i<TREEMANAGER_MAXLEVEL + 1; i++)
		iPow[i] = iPow[i - 1] * 8;

	// Levelレベル（0基点）の配列作成
	cellNum = (iPow[Level + 1] - 1) / 7;
	ppCellAry = new Cell*[cellNum];
	ZeroMemory(ppCellAry, sizeof(Cell*)*cellNum);

	// 領域を登録
	regMin = Min;
	regMax = Max;
	width = XMFLOAT3(regMax.x - regMin.x, regMax.y - regMin.y, regMax.z - regMin.z);
	float tmp = ((float)(1 << Level));
	unit = XMFLOAT3(width.x / tmp, width.y / tmp, width.z / tmp);

	level = Level;

	return true;
}

bool SpaceDivision::Regist(XMFLOAT3 * Min, XMFLOAT3 * Max, ObjectTree * pOT)
{
	// オブジェクトの境界範囲から登録モートン番号を算出
	DWORD Elem = GetMortonNumber(Min, Max);
	if (Elem < cellNum) {
		// 空間が無い場合は新規作成
		if (!ppCellAry[Elem])
			CreateNewCell(Elem);
		return ppCellAry[Elem]->Push(pOT);
	}
	return false;	// 登録失敗
}

DWORD SpaceDivision::GetAllCollisionList(vector<Collider*>& ColVect)
{
	// リスト（配列）は必ず初期化します
	ColVect.clear();

	// ルート空間の存在をチェック
	if (ppCellAry[0] == nullptr)
		return 0;	// 空間が存在していない

					// ルート空間を処理
	list<Collider*> ColStac;
	GetCollisionList(0, ColVect, ColStac);

	return (DWORD)ColVect.size();
}

bool SpaceDivision::GetCollisionList(DWORD Elem, vector<Collider*>& ColVect, list<Collider*>& ColStac)
{
	list<Collider*>::iterator it;
	// ① 空間内のオブジェクト同士の衝突リスト作成
	ObjectTree* pOT1 = ppCellAry[Elem]->GetFirstObj();
	while (pOT1 != false)
	{
		ObjectTree* pOT2 = pOT1->pNext;
		while (pOT2 != false) 
		{
			if (LayerSetting::Instance().CheckRelationship(pOT1->pCol->GetGameObject()->layer, pOT2->pCol->GetGameObject()->layer))
			{	// 衝突リスト作成
				ColVect.push_back(pOT1->pCol);
				ColVect.push_back(pOT2->pCol);
			}

			pOT2 = pOT2->pNext;
		}
		// ② 衝突スタックとの衝突リスト作成
		for (it = ColStac.begin(); it != ColStac.end(); it++)
		{
			if (LayerSetting::Instance().CheckRelationship(pOT1->pCol->GetGameObject()->layer, (*it)->GetGameObject()->layer))
			{
				ColVect.push_back(pOT1->pCol);
				ColVect.push_back(*it);
			}
		}
		pOT1 = pOT1->pNext;
	}

	bool ChildFlag = false;
	// ③ 子空間に移動
	DWORD ObjNum = 0;
	DWORD i, NextElem;
	for (i = 0; i<8; i++) {
		NextElem = Elem * 8 + 1 + i;
		if (NextElem<cellNum && ppCellAry[Elem * 8 + 1 + i]) {
			if (!ChildFlag) {
				// ④ 登録オブジェクトをスタックに追加
				pOT1 = ppCellAry[Elem]->GetFirstObj();
				while (pOT1) {
					ColStac.push_back(pOT1->pCol);
					ObjNum++;
					pOT1 = pOT1->pNext;
				}
			}
			ChildFlag = true;
			GetCollisionList(Elem * 8 + 1 + i, ColVect, ColStac);	// 子空間へ
		}
	}

	// ⑤ スタックからオブジェクトを外す
	if (ChildFlag) {
		for (i = 0; i<ObjNum; i++)
			ColStac.pop_back();
	}

	return true;
}

bool SpaceDivision::CreateNewCell(DWORD Elem)
{
	// 引数の要素番号
	while (!ppCellAry[Elem])
	{
		// 指定の要素番号に空間を新規作成
		ppCellAry[Elem] = NEW Cell();

		// 親空間にジャンプ
		Elem = (Elem - 1) >> 3;
		if (Elem >= cellNum) break;
	}
	return true;
}

DWORD SpaceDivision::GetMortonNumber(XMFLOAT3 * Min, XMFLOAT3 * Max)
{
	// 最小レベルにおける各軸位置を算出
	DWORD LT = GetPointElem(*Min);
	DWORD RB = GetPointElem(*Max);

	// 空間番号を引き算して
	// 最上位区切りから所属レベルを算出
	DWORD Def = RB ^ LT;
	unsigned int HiLevel = 1;
	unsigned int i;
	for (i = 0; i<level; i++)
	{
		DWORD Check = (Def >> (i * 3)) & 0x7;
		if (Check != 0)
			HiLevel = i + 1;
	}
	DWORD SpaceNum = RB >> (HiLevel * 3);
	DWORD AddNum = (iPow[level - HiLevel] - 1) / 7;
	SpaceNum += AddNum;

	if (SpaceNum > cellNum)
		return 0xffffffff;

	return SpaceNum;
}

DWORD SpaceDivision::BitSeparateFor3D(BYTE n)
{
	DWORD s = n;
	s = (s | s << 8) & 0x0000f00f;
	s = (s | s << 4) & 0x000c30c3;
	s = (s | s << 2) & 0x00249249;
	return s;
}

DWORD SpaceDivision::Get3DMortonNumber(BYTE x, BYTE y, BYTE z)
{
	return BitSeparateFor3D(x) | BitSeparateFor3D(y) << 1 | BitSeparateFor3D(z) << 2;
}

DWORD SpaceDivision::GetPointElem(XMFLOAT3 & p)
{
	return Get3DMortonNumber(
		(BYTE)((p.x - regMin.x) / unit.x),
		(BYTE)((p.y - regMin.y) / unit.y),
		(BYTE)((p.z - regMin.z) / unit.z)
	);
}



// class Cell
Cell::~Cell()
{
	if (pLatest != nullptr)
		ResetLink(pLatest);
}

void Cell::ResetLink(ObjectTree * pOT)
{
	if (pOT->pNext != nullptr)
		ResetLink(pOT->pNext);

	pOT = nullptr;
}

bool Cell::Push(ObjectTree * pOT)
{
	if (pOT == nullptr)
		return false;

	if (pOT->pCell == this)
		return false;

	if (pLatest == nullptr)
	{
		pLatest = pOT;
	}
	else
	{
		pOT->pNext = pLatest;
		pLatest->pPre = pOT;
		pLatest = pOT;
	}

	pOT->RegistCell(this);
	return true;
}

ObjectTree * Cell::GetFirstObj()
{
	return pLatest;
}

bool Cell::OnRemove(ObjectTree * pRemoveObj)
{
	if (pLatest == pRemoveObj)
	{
		if (pLatest != nullptr)
			pLatest = pLatest->GetNextObj();
	}

	return true;
}
