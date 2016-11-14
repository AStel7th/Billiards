#pragma once
#include <unordered_map>
#include <vector>
#include "func.h"

using namespace std;

enum struct CALL_TAG
{
	BALL,
	TABLE
};

template<class TYPE = GameObject*>
class _GameObject_
{
protected:
	static TYPE pBegin;
	static TYPE pEnd;
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


class GameObject : public _GameObject_<>
{
private:
	GameObject*			pPrev;	//���g�̑O�|�C���^
	GameObject*			pNext;	//���g�̌�|�C���^

	DestroyMode	mode;		//������i
							
	//�V�X�e���ւ̓o�^
	static inline void _Register_(GameObject* pObj);

	//�V�X�e����������@���̃|�C���^���Ԃ����
	static inline GameObject* _Unregister_(GameObject* pObj);

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