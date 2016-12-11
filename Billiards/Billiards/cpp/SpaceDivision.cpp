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
	// �ݒ�ō����x���ȏ�̋�Ԃ͍��Ȃ�
	if (Level >= TREEMANAGER_MAXLEVEL)
		return false;

	// �e���x���ł̋�Ԑ����Z�o
	int i;
	iPow[0] = 1;
	for (i = 1; i<TREEMANAGER_MAXLEVEL + 1; i++)
		iPow[i] = iPow[i - 1] * 8;

	// Level���x���i0��_�j�̔z��쐬
	cellNum = (iPow[Level + 1] - 1) / 7;
	ppCellAry = new Cell*[cellNum];
	ZeroMemory(ppCellAry, sizeof(Cell*)*cellNum);

	// �̈��o�^
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
	// �I�u�W�F�N�g�̋��E�͈͂���o�^���[�g���ԍ����Z�o
	DWORD Elem = GetMortonNumber(Min, Max);
	if (Elem < cellNum) {
		// ��Ԃ������ꍇ�͐V�K�쐬
		if (!ppCellAry[Elem])
			CreateNewCell(Elem);
		return ppCellAry[Elem]->Push(pOT);
	}
	return false;	// �o�^���s
}

DWORD SpaceDivision::GetAllCollisionList(vector<Collider*>& ColVect)
{
	// ���X�g�i�z��j�͕K�����������܂�
	ColVect.clear();

	// ���[�g��Ԃ̑��݂��`�F�b�N
	if (ppCellAry[0] == nullptr)
		return 0;	// ��Ԃ����݂��Ă��Ȃ�

					// ���[�g��Ԃ�����
	list<Collider*> ColStac;
	GetCollisionList(0, ColVect, ColStac);

	return (DWORD)ColVect.size();
}

bool SpaceDivision::GetCollisionList(DWORD Elem, vector<Collider*>& ColVect, list<Collider*>& ColStac)
{
	list<Collider*>::iterator it;
	// �@ ��ԓ��̃I�u�W�F�N�g���m�̏Փ˃��X�g�쐬
	ObjectTree* pOT1 = ppCellAry[Elem]->GetFirstObj();
	while (pOT1 != false)
	{
		ObjectTree* pOT2 = pOT1->pNext;
		while (pOT2 != false) 
		{
			if (LayerSetting::Instance().CheckRelationship(pOT1->pCol->GetGameObject()->layer, pOT2->pCol->GetGameObject()->layer))
			{	// �Փ˃��X�g�쐬
				ColVect.push_back(pOT1->pCol);
				ColVect.push_back(pOT2->pCol);
			}

			pOT2 = pOT2->pNext;
		}
		// �A �Փ˃X�^�b�N�Ƃ̏Փ˃��X�g�쐬
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
	// �B �q��ԂɈړ�
	DWORD ObjNum = 0;
	DWORD i, NextElem;
	for (i = 0; i<8; i++) {
		NextElem = Elem * 8 + 1 + i;
		if (NextElem<cellNum && ppCellAry[Elem * 8 + 1 + i]) {
			if (!ChildFlag) {
				// �C �o�^�I�u�W�F�N�g���X�^�b�N�ɒǉ�
				pOT1 = ppCellAry[Elem]->GetFirstObj();
				while (pOT1) {
					ColStac.push_back(pOT1->pCol);
					ObjNum++;
					pOT1 = pOT1->pNext;
				}
			}
			ChildFlag = true;
			GetCollisionList(Elem * 8 + 1 + i, ColVect, ColStac);	// �q��Ԃ�
		}
	}

	// �D �X�^�b�N����I�u�W�F�N�g���O��
	if (ChildFlag) {
		for (i = 0; i<ObjNum; i++)
			ColStac.pop_back();
	}

	return true;
}

bool SpaceDivision::CreateNewCell(DWORD Elem)
{
	// �����̗v�f�ԍ�
	while (!ppCellAry[Elem])
	{
		// �w��̗v�f�ԍ��ɋ�Ԃ�V�K�쐬
		ppCellAry[Elem] = NEW Cell();

		// �e��ԂɃW�����v
		Elem = (Elem - 1) >> 3;
		if (Elem >= cellNum) break;
	}
	return true;
}

DWORD SpaceDivision::GetMortonNumber(XMFLOAT3 * Min, XMFLOAT3 * Max)
{
	// �ŏ����x���ɂ�����e���ʒu���Z�o
	DWORD LT = GetPointElem(*Min);
	DWORD RB = GetPointElem(*Max);

	// ��Ԕԍ��������Z����
	// �ŏ�ʋ�؂肩�珊�����x�����Z�o
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
