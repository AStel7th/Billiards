#pragma once
#include <unordered_map>
#include <vector>
#include "func.h"

using namespace std;

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
	GameObject*			pPrev;	//���g�̑O�|�C���^
	GameObject*			pNext;	//���g�̌�|�C���^

	DestroyMode	mode;		//������i
							
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