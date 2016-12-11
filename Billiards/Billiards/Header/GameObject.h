#pragma once
#include <unordered_map>
#include <vector>
#include "func.h"

using namespace std;

class Component;

const float GRAVITY = 9.8f;

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

class Transform
{
public:
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;
	XMFLOAT4X4 matrix;

	Transform() : pos(0.0f, 0.0f, 0.0f), rot(0.0f, 0.0f, 0.0f), scale(1.0f, 1.0f, 1.0f) {}
	~Transform(){}
};

//ゲーム内のオブジェクトの元になるクラス
//各オブジェクトはこのクラスを継承して作成する
class GameObject : public _GameObject_<>
{
private:
	GameObject*		pPrev;	//自身の前ポインタ
	GameObject*		pNext;	//自身の後ポインタ
	DestroyMode		mode;	//消去手段
	vector<Component*> componentList;
	vector<GameObject*> childObjectList;
	bool activeFlg;
	//システムへの登録
	static inline void _Register_(GameObject* pObj);

	//システムから消去　次のポインタが返される
	static inline GameObject* _Unregister_(GameObject* pObj);

	virtual void Update() = 0;

	void RemoveChild(GameObject* pObj);

protected:
	GameObject();
	GameObject(const DestroyMode & mode);
	virtual ~GameObject();

	void SetTag(const string& t);

	void SetName(const string& n);

	void SetLayer(const string& l);
	
public:
	string name;
	string tag;
	string layer;
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;
	XMFLOAT4X4 localMat;
	XMFLOAT4X4 worldMat;
	/*Transform local;
	Transform world;*/
	GameObject* pParent;

	GameObject(const GameObject & gameObject) = delete;

	void AddComponent(Component* pCom);

	vector<Component*> GetComponentList();

	void SetWorld();

	XMFLOAT3 GetWorldPos();

	//void SetWorldPos(XMFLOAT3& pos);
	//void SetWorldPos(XMVECTOR& pos);

	void SetActive(bool flg);

	void AddChild(GameObject* child);

	bool isActive();

	virtual void Destroy();

	virtual bool isDestroy();

	class All
	{
	public:
		static void Clear();

		static void Update();

		static GameObject* GameObjectFindWithName(const string& n);
	};
};

template <class TYPE, typename ... ARGS>
static TYPE* Create(ARGS && ... args)
{
	return NEW TYPE(std::forward<ARGS>(args) ...);
}

template<class T>
static T* GetComponent(GameObject* pObj)
{
	for each (Component* var in pObj->GetComponentList())
	{
		// static_castでキャストするため、事前に型確認が必要
		for each (type_index value in var->id)
		{
			const char* a = value.name();
			const char* b = typeid(T*).name();
			if (value == typeid(T*))
				return static_cast<T*>(var);
		}
	}

	return nullptr;
}