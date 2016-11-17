#include "../Header/Collider.h"
#include "../Header/GameObject.h"
#include "../Header/Component.h"
#include "../Header/SpaceDivision.h"
#include "../Header/func.h"
#include "../Header/Collision.h"
#include "../Header/CollisionFromFBX.h"




// 壁と球の反射ベクトルを計算
// GetRefrectVelo(反射ベクトル,壁の法線,移動ベクトル,壁の反発係数)
void GetRefrectVelo(XMFLOAT3 *pOut, XMFLOAT3 &N, XMFLOAT3 &V, float e)
{
	XMVECTOR valN = XMLoadFloat3(&N);
	XMVECTOR valV = XMLoadFloat3(&V);
	valN = XMVector3Normalize(valN);
	XMStoreFloat3(pOut,valV - (1 + e)*XMVector3Dot(valN, valV)*valN);
}


// 壁との反射後の位置を算出
void GetRelectedPos(float Res_time, Collider &c, XMFLOAT3 &RefV)
{
	PhysicsComponent* pPC = GetComponent<PhysicsComponent>(c.GetGameObject());
	XMVECTOR p = XMLoadFloat3(&c.GetGameObject()->pos);
	XMVECTOR pre_p = XMLoadFloat3(&pPC->prePos);
	XMVECTOR v = XMLoadFloat3(&pPC->velocity);

	// 衝突位置
	// 0.99は壁にめり込まないための補正
	p = pre_p + v * (1 - Res_time)*0.99f;
	// 反射ベクトル
	v = XMLoadFloat3(&RefV);
	// 位置を補正
	p += v * Res_time;

	XMStoreFloat3(&c.GetGameObject()->pos, p);
}

//
//
//// 2球衝突処理
//void SphereColProc(Collider* c1, Collider *s2)
//{
//	float t = 0;
//	D3DXVECTOR3 C1ColPos, C2ColPos, C1Velo, C2Velo;
//
//	// 衝突している2円の衝突位置を検出
//	if (!CheckParticleCollision(
//		s1->r, s2->r,
//		&s1->Pre_p, &s1->p,
//		&s2->Pre_p, &s2->p,
//		&t,
//		&C1ColPos,
//		&C2ColPos))
//		return;	// 衝突していないようです
//
//				// 衝突位置を前位置として保存
//	s1->p = C1ColPos;
//	s2->p = C2ColPos;
//	s1->Pre_p = C1ColPos;
//	s2->Pre_p = C2ColPos;
//
//	// 衝突後の速度を算出
//	if (!CalcParticleColliAfterPos(
//		&C1ColPos, &s1->v,
//		&C2ColPos, &s2->v,
//		s1->w, s2->w,
//		g_Circle_Ref, g_Circle_Ref,		// 球の反発係数
//		t,
//		&C1ColPos, &C1Velo,
//		&C2ColPos, &C2Velo))
//		return; // 何か失敗したようです
//
//				// 衝突後位置に移動
//	s1->v = C1Velo;
//	s2->v = C2Velo;
//	s1->p += s1->v;
//	s2->p += s2->v;
//}

// TODO::ボールのPhysicsコンポーネントで管理
//// 次の球の位置を取得
//void GetNextPos(Collider &c)
//{
//	XMFLOAT3 RefV;	// 反射後の速度ベクトル
//	D3DXVECTOR3 ColliPos;	// 衝突位置
//	float Res_time = 0.0f;	// 衝突後の移動可能時間
//
//							// 重力を掛けて落とす
//	c.GetPhysics()->velocity.y -= g_Gravity / 60;	// 1フレームで9.8/60(m/s)加速
//
//								// 今の速度で位置を更新
//	c.GetPhysics()->prePos = c.GetGameObject()->pos;		// 前の位置を保存
//	c.GetGameObject()->pos += s.v;			// 位置更新
//
//						// 壁との衝突をチェック
//						// X左壁
//	if (s.p.x<s.r && g_XLeft) {
//		// 反射後の速度ベクトルを取得
//		GetRefrectVelo(&RefV, D3DXVECTOR3(1, 0, 0), s.v, Wall_Ref);
//		// 残り時間算出
//		Res_time = (s.p.x - s.r) / s.v.x;
//		// 反射後の位置を算出
//		GetRelectedPos(Res_time, s, RefV);
//	}
//	// X右壁
//	else if (s.p.x>640 - s.r && g_XRight) {
//		GetRefrectVelo(&RefV, D3DXVECTOR3(-1, 0, 0), s.v, Wall_Ref);
//		Res_time = (s.p.x - 640 + s.r) / s.v.x;
//		GetRelectedPos(Res_time, s, RefV);
//	}
//	// Z手前壁
//	if (s.p.z<s.r && g_ZNear) {
//		GetRefrectVelo(&RefV, D3DXVECTOR3(0, 0, 1), s.v, Wall_Ref);
//		Res_time = (s.p.z - s.r) / s.v.z;
//		GetRelectedPos(Res_time, s, RefV);
//	}
//	// Z奥壁
//	else if (s.p.z>640 - s.r && g_ZFar) {
//		GetRefrectVelo(&RefV, D3DXVECTOR3(0, 0, -1), s.v, Wall_Ref);
//		Res_time = (s.p.z - 640 + s.r) / s.v.z;
//		GetRelectedPos(Res_time, s, RefV);
//	}
//	// Y下壁
//	else if (s.p.y<s.r) {
//		GetRefrectVelo(&RefV, D3DXVECTOR3(0, 1, 0), s.v, Wall_Ref);
//		Res_time = (s.p.y - s.r) / s.v.y;
//		GetRelectedPos(Res_time, s, RefV);
//	}
//}


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

Collider::Collider() : pPrev(nullptr), pNext(nullptr),pObject(nullptr)
{
	
}

Collider::~Collider()
{

}

void Collider::Create(GameObject * pObj, ColliderType type)
{
	Collider::_Register_(this);
	pObject = pObj;
	colType = type;

	pObjectTree = NEW ObjectTree();
	pObjectTree->pCol = this;
}

GameObject * Collider::GetGameObject()
{
	return pObject;
}

void Collider::Destroy()
{
	pObjectTree->Remove();
	SAFE_DELETE(pObjectTree);
	Collider::_Unregister_(this);
}


void Collider::All::HitCheck()
{
	vector<Collider*> colVect;
	DWORD colNum = SpaceDivision::Instance().GetAllCollisionList(colVect);

	DWORD c;
	colNum /= 2;

	for (c = 0; c < colNum; c++)
	{
		colVect[c * 2]->CollisionDetection(colVect[c * 2 + 1]);
	}
}


//////////////////////////////////////////////////////
// class SphereCollider
//////////////////////////////////////////////////////
void SphereCollider::Create(GameObject * pObj, ColliderType type,float r)
{
	Collider::_Register_(this);
	pObject = pObj;
	colType = type;
	radius = r;

	pObjectTree = NEW ObjectTree();
	pObjectTree->pCol = this;
}

void SphereCollider::Update()
{
	pObjectTree->Remove();		// 一度リストから外れる
	
	// 再登録
	XMFLOAT3 tmpMin(pObject->pos.x - radius, pObject->pos.y - radius, pObject->pos.z - radius);
	XMFLOAT3 tmpMax(pObject->pos.x + radius, pObject->pos.y + radius, pObject->pos.z + radius);
	SpaceDivision::Instance().Regist(&tmpMin, &tmpMax, pObjectTree);
}

void SphereCollider::CollisionDetection(Collider* pCol)
{
	pCol->isCollision(this);
}

void SphereCollider::isCollision(SphereCollider * other)
{
	//球と球の当たり判定
	float t = 0;
	XMVECTOR C1ColPos, C2ColPos, C1Velo, C2Velo;

	PhysicsComponent* myPC = GetComponent<PhysicsComponent>(pObject);
	PhysicsComponent* otherPC = GetComponent<PhysicsComponent>(other->GetGameObject());

	XMVECTOR myPrePos = XMLoadFloat3(&myPC->prePos);
	XMVECTOR myPos = XMLoadFloat3(&pObject->pos);
	XMVECTOR otherPrePos = XMLoadFloat3(&otherPC->prePos);
	XMVECTOR otherPos = XMLoadFloat3(&other->GetGameObject()->pos);

	// 衝突している2円の衝突位置を検出
	if (!Collision::CheckParticleCollision(
		radius, other->radius,
		&myPrePos, &myPos,
		&otherPrePos, &otherPos,
		&t,
		&C1ColPos,
		&C2ColPos))
		return;	// 衝突していないようです

				// 衝突位置を前位置として保存
	myPos = C1ColPos;
	otherPos = C2ColPos;
	myPrePos = C1ColPos;
	otherPrePos = C2ColPos;

	XMVECTOR myVelo = XMLoadFloat3(&myPC->velocity);
	XMVECTOR otherVelo = XMLoadFloat3(&otherPC->velocity);
	float myMass = myPC->mass;
	float otherMass = otherPC->mass;


	// 衝突後の速度を算出
	if (!Collision::CalcParticleColliAfterPos(
		&C1ColPos, &myVelo,
		&C2ColPos, &otherVelo,
		myMass, otherMass,
		SPHERE_REPULSION, SPHERE_REPULSION,		// 球の反発係数
		t,
		&C1ColPos, &C1Velo,
		&C2ColPos, &C2Velo))
		return; // 何か失敗したようです

				// 衝突後位置に移動
	myVelo = C1Velo;
	otherVelo = C2Velo;
	myPos += myVelo;
	otherPos += otherVelo;

	XMStoreFloat3(&pObject->pos, myPos);
	XMStoreFloat3(&other->GetGameObject()->pos, otherPos);
	XMStoreFloat3(&myPC->prePos, myPrePos);
	XMStoreFloat3(&otherPC->prePos, otherPrePos);
	XMStoreFloat3(&myPC->velocity, myVelo);
	XMStoreFloat3(&otherPC->velocity, otherVelo);

}

void SphereCollider::isCollision(BoxCollider * other)
{
	int a = 0;
}

void SphereCollider::isCollision(MeshCollider * other)
{
	////球とメッシュの当たり判定
	//float t = 0;
	//XMVECTOR ColPos,C1Velo;

	//vector<NODE_COLLISION> nodeCol = other->GetMesh()->GetMeshData();

	//PhysicsComponent* myPC = GetComponent<PhysicsComponent>(pObject);
	//PhysicsComponent* otherPC = GetComponent<PhysicsComponent>(other->GetGameObject());

	//XMVECTOR myPrePos = XMLoadFloat3(&myPC->prePos);
	//XMVECTOR myPos = XMLoadFloat3(&pObject->pos);
	//XMVECTOR otherPrePos = XMLoadFloat3(&otherPC->prePos);
	//XMVECTOR otherPos = XMLoadFloat3(&other->GetGameObject()->pos);

	//for (int i = 0; i < nodeCol.size(); i++)
	//{
	//	if (nodeCol[i].m_polyDataArray.size() == 0)
	//		continue;

	//	for (int j = 0; j < nodeCol[i].m_polyDataArray.size(); j++)
	//	{
	//		XMVECTOR normal = XMLoadFloat3(&nodeCol[i].m_polyDataArray[j].normal);
	//		XMVECTOR pos[3] = 
	//		{
	//			XMLoadFloat3(&nodeCol[i].m_polyDataArray[j].vertexArray[0].vPos),
	//			XMLoadFloat3(&nodeCol[i].m_polyDataArray[j].vertexArray[1].vPos),
	//			XMLoadFloat3(&nodeCol[i].m_polyDataArray[j].vertexArray[2].vPos)
	//		};

	//		// 衝突している円とメッシュの衝突位置を検出
	//		if (!Collision::SpherePolygonCollision(
	//			radius,
	//			&myPrePos, &myPos,
	//			&normal, pos,
	//			&t,
	//			&ColPos))
	//			return;	// 衝突していないようです
	//	}
	//}
	//

	//			// 衝突位置を前位置として保存
	//myPos = ColPos;
	//myPrePos = ColPos;

	//XMVECTOR myVelo = XMLoadFloat3(&myPC->velocity);
	//XMVECTOR otherVelo = XMLoadFloat3(&otherPC->velocity);
	//float myMass = myPC->mass;
	//float otherMass = otherPC->mass;


	//// 衝突後の速度を算出
	//if (!Collision::CalcParticleColliAfterPos(
	//	&C1ColPos, &myVelo,
	//	&C2ColPos, &otherVelo,
	//	myMass, otherMass,
	//	SPHERE_REPULSION, SPHERE_REPULSION,		// 球の反発係数
	//	t,
	//	&C1ColPos, &C1Velo,
	//	&C2ColPos, &C2Velo))
	//	return; // 何か失敗したようです

	//			// 衝突後位置に移動
	//myVelo = C1Velo;
	//otherVelo = C2Velo;
	//myPos += myVelo;
	//otherPos += otherVelo;

	//XMStoreFloat3(&pObject->pos, myPos);
	//XMStoreFloat3(&other->GetGameObject()->pos, otherPos);
	//XMStoreFloat3(&myPC->prePos, myPrePos);
	//XMStoreFloat3(&otherPC->prePos, otherPrePos);
	//XMStoreFloat3(&myPC->velocity, myVelo);
	//XMStoreFloat3(&otherPC->velocity, otherVelo);
}


//////////////////////////////////////////////////////
// class BoxCollider
//////////////////////////////////////////////////////
void BoxCollider::Create(GameObject * pObj, ColliderType type, float s)
{
	Collider::_Register_(this);
	pObject = pObj;
	colType = type;
	size = s;

	pObjectTree = NEW ObjectTree();
	pObjectTree->pCol = this;
}

void BoxCollider::Update()
{
	pObjectTree->Remove();		// 一度リストから外れる

								// 再登録
	XMFLOAT3 tmpMin(pObject->pos.x - size, pObject->pos.y - size, pObject->pos.z - size);
	XMFLOAT3 tmpMax(pObject->pos.x + size, pObject->pos.y + size, pObject->pos.z + size);
	SpaceDivision::Instance().Regist(&tmpMin, &tmpMax, pObjectTree);
}

void BoxCollider::CollisionDetection(Collider * pCol)
{
	pCol->isCollision(this);
}

void BoxCollider::isCollision(SphereCollider * other)
{
	int a = 0;
}

void BoxCollider::isCollision(BoxCollider * other)
{
	int a = 0;
}

void BoxCollider::isCollision(MeshCollider * other)
{
	int a = 0;
}




//////////////////////////////////////////////////////
// class MeshCollider
//////////////////////////////////////////////////////
MeshCollider::~MeshCollider()
{
	SAFE_DELETE(pMeshCol);
}
void MeshCollider::Create(GameObject* pObj, ColliderType type, const char* fpath, float s)
{
	Collider::_Register_(this);
	pObject = pObj;
	colType = type;
	size = s;

	pMeshCol = NEW CollisionFromFBX();
	pMeshCol->LoadFBX(fpath);

	pObjectTree = NEW ObjectTree();
	pObjectTree->pCol = this;
}

void MeshCollider::Update()
{
	pObjectTree->Remove();		// 一度リストから外れる

								// 再登録
	XMFLOAT3 tmpMin(pObject->pos.x - size, pObject->pos.y - size, pObject->pos.z - size);
	XMFLOAT3 tmpMax(pObject->pos.x + size, pObject->pos.y + size, pObject->pos.z + size);
	SpaceDivision::Instance().Regist(&tmpMin, &tmpMax, pObjectTree);
}

CollisionFromFBX * MeshCollider::GetMesh()
{
	return pMeshCol;
}

void MeshCollider::CollisionDetection(Collider * pCol)
{
	pCol->isCollision(this);
}

void MeshCollider::isCollision(SphereCollider * other)
{
	int a = 0;
}

void MeshCollider::isCollision(BoxCollider * other)
{
	int a = 0;
}

void MeshCollider::isCollision(MeshCollider * other)
{
	int a = 0;
}
