#include "../Header/GameObject.h"

//システムへの登録
inline void GameObject::_Register_(GameObject* pObj)
{
	//先頭が空の場合は新規タスクを設定
	if (pBegin == nullptr) pBegin = pObj;
	else
	{
		//新規タスクの前に末尾タスクを代入
		pObj->pPrev = pEnd;

		//末尾タスクの次に新規タスクを代入
		pEnd->pNext = pObj;
	}

	//末尾タスクが新規タスクになる
	pEnd = pObj;
}

//システムから消去　次のポインタが返される
inline GameObject* GameObject::_Unregister_(GameObject* pObj)
{
	GameObject* next = pObj->pNext; //自身の次
	GameObject* prev = pObj->pPrev; //自身の前

									//次位置に前位置を代入してつなぎ合わせる
	if (next != nullptr) next->pPrev = prev;
	//null の場合は末尾タスクがなくなったので、前のタスクを末尾にする
	else pEnd = prev;

	//前に次を代入してつなぎ合わせる
	if (prev != nullptr) prev->pNext = next;
	//null の場合は先頭タスクがなくなったので、次のタスクを先頭にする
	else pBegin = next;

	if (pObj->childObjectList.size() != 0)
	{
		for(auto it = pObj->childObjectList.begin(); it != pObj->childObjectList.end();++it)
			GameObject::_Unregister_(*it);
	}

	//タスクの消去
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
	//次の更新で消滅するように設定
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
	GameObject* pObj = pBegin; //現在のタスク

							 //末尾までループする
	while (pObj != nullptr)
	{
		if (pObj->pParent != nullptr)
			pObj->pParent->RemoveChild(pObj);

		pObj = GameObject::_Unregister_(pObj);
	}
}

void GameObject::All::Update()
{
	GameObject* pObj = pBegin; //現在のタスク

							 //末尾までループする
	while (pObj != nullptr)
	{
		//次のオブジェクトを事前取得
		GameObject* _next = pObj->pNext;

		//タスクの消去
		switch (pObj->mode)
		{
		case DestroyMode::None:
			if(pObj->isActive())
				pObj->Update();
			break;

		case DestroyMode::Destroy:
			//Task::All::Update が呼び出された時に消去
			if (pObj->pParent != nullptr)
				pObj->pParent->RemoveChild(pObj);
			GameObject::_Unregister_(pObj);
			break;

		default:
			assert(!"DestroyMode error");
			break;
		}

		//次のタスクへ移動
		pObj = _next;
	}
}

GameObject * GameObject::All::GameObjectFindWithName(const string & n)
{
	GameObject* pObj = pBegin; //現在のタスク
	
	//末尾までループする
	while (pObj != nullptr)
	{
		//次のオブジェクトを事前取得
		GameObject* _next = pObj->pNext;

		if (pObj->name == n)
			return pObj;

		//次のタスクへ移動
		pObj = _next;
	}

	return nullptr;
}
