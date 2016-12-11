#pragma once
#include <unordered_map>
#include <vector>
#include <DirectXMath.h>

const float SPHERE_REPULSION = 0.85f;
const float WALL_REPULSION = 0.75f;		// 球と壁の反発係数

using namespace std;
using namespace DirectX;

class GameObject;
class SpaceDivision;
class ObjectTree;
class PhysicsComponent;
class SphereCollider;
class BoxCollider;
class MeshCollider;
class CollisionFromFBX;

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

enum ColliderType
{
	Sphere,
	Box,
	Mesh,
};


class Collider : public _Collider_<>
{
protected:
	Collider*			pPrev;		//自身の前ポインタ
	Collider*			pNext;		//自身の後ポインタ
	ObjectTree*			pObjectTree;//空間分割用ノードクラス
	GameObject*			pObject;
	ColliderType		colType;
	vector<string>		targetTagList;
	PhysicsComponent*	pPhysics;
	//システムへの登録
	static inline void _Register_(Collider* pObj);

	//システムから消去　次のポインタが返される
	static inline Collider* _Unregister_(Collider* pObj);
	
public:
	Collider();
	virtual ~Collider();

	virtual void Create(GameObject* pObj,ColliderType type);

	virtual void Update() = 0;

	GameObject* GetGameObject();

	PhysicsComponent* GetPhysics();

	void AddTargetTag(const string& t);
	
	virtual void Destroy();

	virtual void CollisionDetection(Collider* pCol) = 0;

	virtual void isCollision(SphereCollider* other) = 0;
	virtual void isCollision(BoxCollider* other) = 0;
	virtual void isCollision(MeshCollider* other) = 0;

	class All
	{
	public:
		static void HitCheck();
	};
};

class SphereCollider : public Collider
{
private:
public:
	float radius;

	SphereCollider() : radius(1.0f){}
	virtual ~SphereCollider(){}

	void Create(GameObject* pObj, ColliderType type, float r);

	void Update();

	void CollisionDetection(Collider* pCol);

	void isCollision(SphereCollider* other);
	void isCollision(BoxCollider* other);
	void isCollision(MeshCollider* other);

};

class BoxCollider : public Collider
{
private:
	XMFLOAT3 center;
	float size;
public:
	XMFLOAT3 minPos;	//BOXの左下
	XMFLOAT3 maxPos;

	BoxCollider() : minPos(), maxPos(),center(),size(1.0f){}
	virtual ~BoxCollider() {}

	void Create(GameObject* pObj, ColliderType type, float s);

	void Update();

	void CollisionDetection(Collider* pCol);

	void isCollision(SphereCollider* other);
	void isCollision(BoxCollider* other);
	void isCollision(MeshCollider* other);
};

class MeshCollider : public Collider
{
private:
	CollisionFromFBX* pMeshCol;
	XMFLOAT4X4 world;
	float size;	//TODO::現在空間登録用の包み込み範囲がメッシュだと指定できていない、今後考えなければならない
public:
	MeshCollider() : pMeshCol(nullptr), size(1.0f){}
	virtual ~MeshCollider();

	void Create(GameObject* pObj, ColliderType type, const char* fpath,float s);

	void Update();

	CollisionFromFBX* GetMesh();

	void CollisionDetection(Collider* pCol);

	void isCollision(SphereCollider* other);
	void isCollision(BoxCollider* other);
	void isCollision(MeshCollider* other);
};