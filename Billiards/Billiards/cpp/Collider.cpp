#include "../Header/Collider.h"
#include "../Header/func.h"

//システムへの登録
inline void Collider::_Register_(Collider* pCol)
{
	//先頭が空の場合は新規タスクを設定
	if (pBegin == nullptr) pBegin = pCol;
	else
	{
		//新規タスクの前に末尾タスクを代入
		pCol->pPrev = pEnd;

		//末尾タスクの次に新規タスクを代入
		pEnd->pNext = pCol;
	}

	//末尾タスクが新規タスクになる
	pEnd = pCol;
}

//システムから消去　次のポインタが返される
inline Collider* Collider::_Unregister_(Collider* pCol)
{
	Collider* next = pCol->pNext; //自身の次
	Collider* prev = pCol->pPrev; //自身の前

									//次位置に前位置を代入してつなぎ合わせる
	if (next != nullptr) next->pPrev = prev;
	//null の場合は末尾タスクがなくなったので、前のタスクを末尾にする
	else pEnd = prev;

	//前に次を代入してつなぎ合わせる
	if (prev != nullptr) prev->pNext = next;
	//null の場合は先頭タスクがなくなったので、次のタスクを先頭にする
	else pBegin = next;

	//タスクの消去
	SAFE_DELETE(pCol);

	//次のタスクを返す
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
	//Collider* pCol = pBegin; //現在のタスク

	//						   //末尾までループする
	//while (pCol != nullptr)
	//{
	//	//次のオブジェクトを事前取得
	//	GameObject* _next = pObj->pNext;

	//	//タスクの消去
	//	switch (pObj->mode)
	//	{
	//	case DestroyMode::None:
	//		pObj->Update();
	//		break;

	//	case DestroyMode::Destroy:
	//		//Task::All::Update が呼び出された時に消去
	//		GameObject::_Unregister_(pObj);
	//		break;

	//	default:
	//		assert(!"DestroyMode error");
	//		break;
	//	}

	//	//次のタスクへ移動
	//	pObj = _next;
	//}
}