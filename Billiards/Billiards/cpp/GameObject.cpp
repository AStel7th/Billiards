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

	//タスクの消去
	SAFE_DELETE(pObj);

	//次のタスクを返す
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
	//次の更新で消滅するように設定
	mode = DestroyMode::Destroy;
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
			//無し
			break;

		case DestroyMode::Destroy:
			//Task::All::Update が呼び出された時に消去
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