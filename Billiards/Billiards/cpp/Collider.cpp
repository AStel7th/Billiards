#include "../Header/Collider.h"
#include "../Header/func.h"

//�V�X�e���ւ̓o�^
inline void Collider::_Register_(Collider* pCol)
{
	//�擪����̏ꍇ�͐V�K�^�X�N��ݒ�
	if (pBegin == nullptr) pBegin = pCol;
	else
	{
		//�V�K�^�X�N�̑O�ɖ����^�X�N����
		pCol->pPrev = pEnd;

		//�����^�X�N�̎��ɐV�K�^�X�N����
		pEnd->pNext = pCol;
	}

	//�����^�X�N���V�K�^�X�N�ɂȂ�
	pEnd = pCol;
}

//�V�X�e����������@���̃|�C���^���Ԃ����
inline Collider* Collider::_Unregister_(Collider* pCol)
{
	Collider* next = pCol->pNext; //���g�̎�
	Collider* prev = pCol->pPrev; //���g�̑O

									//���ʒu�ɑO�ʒu�������ĂȂ����킹��
	if (next != nullptr) next->pPrev = prev;
	//null �̏ꍇ�͖����^�X�N���Ȃ��Ȃ����̂ŁA�O�̃^�X�N�𖖔��ɂ���
	else pEnd = prev;

	//�O�Ɏ��������ĂȂ����킹��
	if (prev != nullptr) prev->pNext = next;
	//null �̏ꍇ�͐擪�^�X�N���Ȃ��Ȃ����̂ŁA���̃^�X�N��擪�ɂ���
	else pBegin = next;

	//�^�X�N�̏���
	SAFE_DELETE(pCol);

	//���̃^�X�N��Ԃ�
	return next;
}

Collider::Collider() : pPrev(nullptr), pNext(nullptr), spaceID(0), pObject(nullptr)
{
	
}

Collider::~Collider()
{

}

void Collider::Create(GameObject * pObj)
{
	Collider::_Register_(this);
	pObject = pObj;
}

void Collider::Update()
{

}

int Collider::GetSpaceID()
{
	return spaceID;
}

void Collider::Destroy()
{
	Collider::_Unregister_(this);
}


void Collider::All::HitCheck()
{
	//Collider* pCol = pBegin; //���݂̃^�X�N

	//						   //�����܂Ń��[�v����
	//while (pCol != nullptr)
	//{
	//	//���̃I�u�W�F�N�g�����O�擾
	//	GameObject* _next = pObj->pNext;

	//	//�^�X�N�̏���
	//	switch (pObj->mode)
	//	{
	//	case DestroyMode::None:
	//		pObj->Update();
	//		break;

	//	case DestroyMode::Destroy:
	//		//Task::All::Update ���Ăяo���ꂽ���ɏ���
	//		GameObject::_Unregister_(pObj);
	//		break;

	//	default:
	//		assert(!"DestroyMode error");
	//		break;
	//	}

	//	//���̃^�X�N�ֈړ�
	//	pObj = _next;
	//}
}