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

// ���؃N���X
class ObjectTree
{
public:
	Cell*		pCell;		// �o�^���
	Collider*	pCol;		// ����ΏۃR���C�_�[
	ObjectTree* pPre;		// �O��ObjectTree�ւ̃|�C���^
	ObjectTree* pNext;		// ����ObjectTree�ւ̃|�C���^

public:
	ObjectTree();

	virtual ~ObjectTree() {}

public:
	// ���烊�X�g����O���
	bool Remove();

	void RegistCell(Cell* pC);

	ObjectTree* GetNextObj();
};


// ���؋�ԊǗ��N���X
class SpaceDivision
{
protected:
	unsigned int	dim;
	Cell**			ppCellAry;						// ���`��ԃ|�C���^�z��
	unsigned int	iPow[TREEMANAGER_MAXLEVEL + 1];	// �ׂ��搔�l�z��
	XMFLOAT3		width;							// �̈�̕�
	XMFLOAT3		regMin;							// �̈�̍ŏ��l
	XMFLOAT3		regMax;							// �̈�̍ő�l
	XMFLOAT3		unit;							// �ŏ��̈�̕ӂ̒���
	DWORD			cellNum;						// ��Ԃ̐�
	unsigned int	level;							// �ŉ��ʃ��x��

	SpaceDivision();

public:

	// �V���O���g���I�u�W�F�N�g���擾
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
	// ��ԓ��ŏՓ˃��X�g���쐬����
	bool GetCollisionList(DWORD Elem, vector<Collider*> &ColVect, list<Collider*> &ColStac);

	// ��Ԃ𐶐�
	bool CreateNewCell(DWORD Elem);

	// ���W�����Ԕԍ����Z�o
	DWORD GetMortonNumber(XMFLOAT3 *Min, XMFLOAT3 *Max);

	// �r�b�g�����֐�
	DWORD BitSeparateFor3D(BYTE n);

	// 3D���[�g����Ԕԍ��Z�o�֐�
	DWORD Get3DMortonNumber(BYTE x, BYTE y, BYTE z);

	// ���W�����`8���ؗv�f�ԍ��ϊ��֐�
	DWORD GetPointElem(XMFLOAT3 &p);
};



// ��ԃN���X
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