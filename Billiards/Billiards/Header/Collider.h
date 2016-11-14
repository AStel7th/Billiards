#pragma once
#include <unordered_map>
#include <vector>

using namespace std;

class GameObject;

template<class TYPE = Collider*>
class _Collider_
{
protected:
	static TYPE pBegin;
	static TYPE pEnd;
};


template<class TYPE>
TYPE _Collider_<TYPE>::pBegin = nullptr;
template<class TYPE>
TYPE _Collider_<TYPE>::pEnd = nullptr;


class Collider : public _Collider_<>
{
private:
	Collider*			pPrev;	//���g�̑O�|�C���^
	Collider*			pNext;	//���g�̌�|�C���^

	int spaceID;
	GameObject* pObject;

	//�V�X�e���ւ̓o�^
	static inline void _Register_(Collider* pObj);

	//�V�X�e����������@���̃|�C���^���Ԃ����
	static inline Collider* _Unregister_(Collider* pObj);
	
public:
	Collider();
	virtual ~Collider();

	void Create(GameObject* pObj);

	void Update();

	int GetSpaceID();
	
	virtual void Destroy();

	class All
	{
	public:
		static void HitCheck();
	};
};
//
//class ColliderContainer
//{
//	static unordered_map<int, vector<Collider*>> colliderList;
//public:
//	static void AddCollider(Collider* pCol);
//	static void HitCheck();
//	static void AllClear();
//};