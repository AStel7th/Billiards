//#pragma once
//#include <unordered_map>
//#include <vector>
//
//using namespace std;
//
//class GameObject;
//
//class Collider
//{
//private:
//	int spaceID;
//	GameObject* pObject = nullptr;
//public:
//	Collider();
//	~Collider();
//
//	void Create(GameObject* pObj);
//
//	int GetSpaceID();
//};
//
//class ColliderContainer
//{
//	static unordered_map<int, vector<Collider*>> colliderList;
//public:
//	static void AddCollider(Collider* pCol);
//	static void HitCheck();
//	static void AllClear();
//};