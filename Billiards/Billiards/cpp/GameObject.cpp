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

	if (pObj->childObjectList.size() != 0)
	{
		for(auto it = pObj->childObjectList.begin(); it != pObj->childObjectList.end();++it)
			GameObject::_Unregister_(*it);
	}

	//�^�X�N�̏���
	SAFE_DELETE(pObj);

	return pBegin;
}

GameObject::GameObject() :
	pPrev(nullptr),
	pNext(nullptr),
	mode(DestroyMode::None),
	pos(0.0f, 0.0f, 0.0f), 
	rot(0.0f, 0.0f, 0.0f), 
	scale(1.0f, 1.0f, 1.0f),
	/*local(),
	world(),*/
	activeFlg(true),
	pParent(nullptr)
{
	name = "";
	tag = "";
	GameObject::_Register_(this);
}

GameObject::~GameObject()
{

}

void GameObject::SetWorld()
{
	XMMATRIX _localMat = XMMatrixIdentity();
	XMMATRIX _worldMat = XMMatrixIdentity();

	_localMat *= XMMatrixScaling(scale.x, scale.y, scale.z);
	_localMat *= XMMatrixRotationX(rot.x);
	_localMat *= XMMatrixRotationY(rot.y);
	_localMat *= XMMatrixRotationZ(rot.z);
	_localMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);

	_worldMat = _localMat;
	if (pParent != nullptr)
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&pParent->worldMat);
		_worldMat = _localMat * parentWorld;
	}

	XMStoreFloat4x4(&localMat, _localMat);
	XMStoreFloat4x4(&worldMat, _worldMat);

	for (auto it = childObjectList.begin(); it != childObjectList.end(); ++it)
	{
		(*it)->SetWorld();
	}
}

XMFLOAT3 GameObject::GetWorldPos()
{
	return XMFLOAT3(worldMat._41, worldMat._42, worldMat._43);
}

//void GameObject::SetWorldPos(XMFLOAT3 & pos)
//{
//	worldMat._41 = pos.x;
//	worldMat._42 = pos.y;
//	worldMat._43 = pos.z;
//}
//
//void GameObject::SetWorldPos(XMVECTOR & pos)
//{
//	XMFLOAT3 p;
//	XMStoreFloat3(&p, pos);
//
//	worldMat._41 = p.x;
//	worldMat._42 = p.y;
//	worldMat._43 = p.z;
//}

void GameObject::SetTag(const string & t)
{
	tag = t;
}

void GameObject::SetName(const string & n)
{
	name = n;
}

void GameObject::SetActive(bool flg)
{
	activeFlg = flg;

	for (auto it = childObjectList.begin(); it != childObjectList.end(); ++it)
	{
		(*it)->SetActive(flg);
	}
}

void GameObject::AddChild(GameObject* child)
{
	childObjectList.push_back(child);
	child->pParent = this;
	child->SetWorld();
}


void GameObject::RemoveChild(GameObject * pObj)
{
	auto it = find(childObjectList.begin(), childObjectList.end(), pObj);

	if (it == childObjectList.end())
		return;
			
	childObjectList.erase(it);
}

bool GameObject::isActive()
{
	return activeFlg;
}

void GameObject::Destroy()
{
	//���̍X�V�ŏ��ł���悤�ɐݒ�
	mode = DestroyMode::Destroy;
}

void GameObject::AddComponent(Component* pCom)
{
	componentList.push_back(pCom);
}

vector<Component*> GameObject::GetComponentList()
{
	return componentList;
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
		if (pObj->pParent != nullptr)
			pObj->pParent->RemoveChild(pObj);

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
			if(pObj->isActive())
				pObj->Update();
			break;

		case DestroyMode::Destroy:
			//Task::All::Update ���Ăяo���ꂽ���ɏ���
			if (pObj->pParent != nullptr)
				pObj->pParent->RemoveChild(pObj);
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

GameObject * GameObject::All::GameObjectFindWithName(const string & n)
{
	GameObject* pObj = pBegin; //���݂̃^�X�N
	
	//�����܂Ń��[�v����
	while (pObj != nullptr)
	{
		//���̃I�u�W�F�N�g�����O�擾
		GameObject* _next = pObj->pNext;

		if (pObj->name == n)
			return pObj;

		//���̃^�X�N�ֈړ�
		pObj = _next;
	}

	return nullptr;
}
