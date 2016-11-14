#include "../Header/GameObject.h"

//�V�X�e���ւ̓o�^
inline void GameObject::_Register_(GameObject* pObj)
{
	//�擪����̏ꍇ�͐V�K�^�X�N��ݒ�
	if (pBegin == nullptr) pBegin = pObj;
	else
	{
		//�V�K�^�X�N�̑O�ɖ����^�X�N����
		pObj->pPrev = pEnd;

		//�����^�X�N�̎��ɐV�K�^�X�N����
		pEnd->pNext = pObj;
	}

	//�����^�X�N���V�K�^�X�N�ɂȂ�
	pEnd = pObj;
}

//�V�X�e����������@���̃|�C���^���Ԃ����
inline GameObject* GameObject::_Unregister_(GameObject* pObj)
{
	GameObject* next = pObj->pNext; //���g�̎�
	GameObject* prev = pObj->pPrev; //���g�̑O

									//���ʒu�ɑO�ʒu�������ĂȂ����킹��
	if (next != nullptr) next->pPrev = prev;
	//null �̏ꍇ�͖����^�X�N���Ȃ��Ȃ����̂ŁA�O�̃^�X�N�𖖔��ɂ���
	else pEnd = prev;

	//�O�Ɏ��������ĂȂ����킹��
	if (prev != nullptr) prev->pNext = next;
	//null �̏ꍇ�͐擪�^�X�N���Ȃ��Ȃ����̂ŁA���̃^�X�N��擪�ɂ���
	else pBegin = next;

	//�^�X�N�̏���
	SAFE_DELETE(pObj);

	//���̃^�X�N��Ԃ�
	return next;
}

GameObject::GameObject() : pPrev(nullptr), pNext(nullptr),mode(DestroyMode::None),posX(0),posY(0),posZ(0),rotX(0),rotY(0),rotZ(0)
{
	GameObject::_Register_(this);
}

GameObject::~GameObject()
{

}

void GameObject::Destroy()
{
	//���̍X�V�ŏ��ł���悤�ɐݒ�
	mode = DestroyMode::Destroy;
}

bool GameObject::isDestroy()
{
	return mode == DestroyMode::Destroy;
}

void GameObject::All::Clear()
{
	GameObject* pObj = pBegin; //���݂̃^�X�N

							 //�����܂Ń��[�v����
	while (pObj != nullptr)
	{
		pObj = GameObject::_Unregister_(pObj);
	}
}

void GameObject::All::Update()
{
	GameObject* pObj = pBegin; //���݂̃^�X�N

							 //�����܂Ń��[�v����
	while (pObj != nullptr)
	{
		//���̃I�u�W�F�N�g�����O�擾
		GameObject* _next = pObj->pNext;

		//�^�X�N�̏���
		switch (pObj->mode)
		{
		case DestroyMode::None:
			//����
			break;

		case DestroyMode::Destroy:
			//Task::All::Update ���Ăяo���ꂽ���ɏ���
			GameObject::_Unregister_(pObj);
			break;

		default:
			assert(!"DestroyMode error");
			break;
		}

		//���̃^�X�N�ֈړ�
		pObj = _next;
	}
}