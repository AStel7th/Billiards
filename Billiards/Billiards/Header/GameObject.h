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
	static TYPE pBegin;			//�A�����X�g�̓�
	static TYPE pEnd;			//�A�����X�g�̖���
};

template<class TYPE>
TYPE _GameObject_<TYPE>::pBegin = nullptr;
template<class TYPE>
TYPE _GameObject_<TYPE>::pEnd = nullptr;


enum DestroyMode
{
	None,		//GameObject::Destroy ���Ăяo���ď���
	Destroy,	//GameObject::Update ���Ăяo���ꂽ���ɏ���
};


//�Q�[�����̃I�u�W�F�N�g�̌��ɂȂ�N���X
//�e�I�u�W�F�N�g�͂��̃N���X���p�����č쐬����
class GameObject : public _GameObject_<>
{
private:
	GameObject*		pPrev;	//���g�̑O�|�C���^
	GameObject*		pNext;	//���g�̌�|�C���^
	DestroyMode		mode;	//������i
	vector<Component*> componentList;
	//�V�X�e���ւ̓o�^
	static inline void _Register_(GameObject* pObj);

	//�V�X�e����������@���̃|�C���^���Ԃ����
	static inline GameObject* _Unregister_(GameObject* pObj);

	virtual void Update() = 0;

protected:
	GameObject();
	GameObject(const DestroyMode & mode);
	virtual ~GameObject();
	
	virtual void Destroy();
public:
	XMFLOAT3 pos;
	XMFLOAT3 rot;
	XMFLOAT3 scale;
	XMFLOAT4X4 world;

	GameObject(const GameObject & gameObject) = delete;

	void AddComponent(Component* pCom);

	vector<Component*> GetComponentList();

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

template<class T>
static T* GetComponent(GameObject* pObj)
{
	for each (Component* var in pObj->GetComponentList())
	{
		// static_cast�ŃL���X�g���邽�߁A���O�Ɍ^�m�F���K�v
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