#pragma once
#include <unordered_map>
#include <vector>
#include "func.h"

using namespace std;

template<class TYPE = GameObject*>
class _GameObject_
{
protected:
	static TYPE pBegin;			//連結リストの頭
	static TYPE pEnd;			//連結リストの末尾
};

template<class TYPE>
TYPE _GameObject_<TYPE>::pBegin = nullptr;
template<class TYPE>
TYPE _GameObject_<TYPE>::pEnd = nullptr;


enum DestroyMode
{
	None,		//GameObject::Destroy を呼び出して消去
	Destroy,	//GameObject::Update が呼び出された時に消去
};


//ゲーム内のオブジェクトの元になるクラス
//各オブジェクトはこのクラスを継承して作成する
class GameObject : public _GameObject_<>
{
private:
	GameObject*			pPrev;	//自身の前ポインタ
	GameObject*			pNext;	//自身の後ポインタ

	DestroyMode	mode;		//消去手段
							
	//システムへの登録
	static inline void _Register_(GameObject* pObj);

	//システムから消去　次のポインタが返される
	static inline GameObject* _Unregister_(GameObject* pObj);

	virtual void Update() = 0;

protected:
	GameObject();
	GameObject(const DestroyMode & mode);
	virtual ~GameObject();
	
	virtual void Destroy();
public:
	float posX, posY, posZ;
	float rotX, rotY, rotZ;

	GameObject(const GameObject & gameObject) = delete;

	virtual bool isDestroy();

	class All
	{
	public:
		static void Clear();

		static void Update();
	};
};

template <class TYPE, typename ... ARGS>
static TYPE* Create(ARGS && ... args)
{
	return NEW TYPE(std::forward<ARGS>(args) ...);
}