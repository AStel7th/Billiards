#include "../Header/Collider.h"
#include "../Header/GameObject.h"
#include "../Header/Component.h"
#include "../Header/SpaceDivision.h"
#include "../Header/func.h"
#include "../Header/Collision.h"
#include "../Header/CollisionFromFBX.h"

void ConvertToParentSpace(XMVECTOR& pos, XMMATRIX& parentWorld)
{
	pos = XMVector3Transform(pos, parentWorld);
}

void ConvertToLocalSpace(XMVECTOR& pos, XMMATRIX& parentWorld)
{
	XMMATRIX invMat = XMMatrixInverse(nullptr, parentWorld);

	pos = XMVector3Transform(pos, invMat);
}

// 壁と球の反射ベクトルを計算
// GetRefrectVelo(反射ベクトル,壁の法線,移動ベクトル,壁の反発係数)
void GetRefrectVelo(XMVECTOR *pOut, XMVECTOR &N, XMVECTOR &V, float e)
{
	N = XMVector3Normalize(N);
	*pOut = V - (1 + e)*XMVector3Dot(N, V)*N;
}


// 壁との反射後の位置を算出
void GetRelectedPos(float Res_time, Collider &c, XMVECTOR& RefV)
{
	XMVECTOR p = XMLoadFloat3(&c.GetGameObject()->pos);
	XMVECTOR pre_p = XMLoadFloat3(&c.GetPhysics()->prePos);
	XMVECTOR v = XMLoadFloat3(&c.GetPhysics()->velocity);

	// 衝突位置
	// 0.99は壁にめり込まないための補正
	p = pre_p + v * (1 - Res_time)*0.99f;
	// 反射ベクトル
	v = RefV;
	// 位置を補正
	p += v * Res_time;

	XMStoreFloat3(&c.GetGameObject()->pos, p);
	XMStoreFloat3(&c.GetPhysics()->velocity, v);
}

void CollisionOfSpheres(SphereCollider* sCol1, SphereCollider* sCol2 )
{
	//球と球の当たり判定
	float t = 0;
	XMVECTOR C1ColPos, C2ColPos, C1Velo, C2Velo;

	XMVECTOR sCol1PrePos = XMLoadFloat3(&sCol1->GetPhysics()->prePos);
	XMVECTOR sCol1Pos = XMLoadFloat3(&sCol1->GetGameObject()->pos);
	if (sCol1->GetGameObject()->pParent != nullptr)
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&sCol1->GetGameObject()->pParent->worldMat);
		ConvertToParentSpace(sCol1Pos, parentWorld);
		ConvertToParentSpace(sCol1PrePos, parentWorld);
	}

	XMVECTOR sCol2PrePos = XMLoadFloat3(&sCol2->GetPhysics()->prePos);
	XMVECTOR sCol2Pos = XMLoadFloat3(&sCol2->GetGameObject()->pos);
	if (sCol2->GetGameObject()->pParent != nullptr)
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&sCol2->GetGameObject()->pParent->worldMat);
		ConvertToParentSpace(sCol2Pos, parentWorld);
		ConvertToParentSpace(sCol2PrePos, parentWorld);
	}

	// 衝突している2円の衝突位置を検出
	if (!Collision::CheckParticleCollision(
		sCol1->radius, sCol2->radius,
		&sCol1PrePos, &sCol1Pos,
		&sCol2PrePos, &sCol2Pos,
		&t,
		&C1ColPos,
		&C2ColPos))
		return;

	// 衝突位置を前位置として保存
	sCol1Pos = C1ColPos;
	sCol2Pos = C2ColPos;
	sCol1PrePos = C1ColPos;
	sCol2PrePos = C2ColPos;

	XMVECTOR sCol1Velo = XMLoadFloat3(&sCol1->GetPhysics()->velocity);
	XMVECTOR sCol2Velo = XMLoadFloat3(&sCol2->GetPhysics()->velocity);
	float sCol1Mass = sCol1->GetPhysics()->mass;
	float sCol2Mass = sCol2->GetPhysics()->mass;


	// 衝突後の速度を算出
	if (!Collision::CalcParticleColliAfterPos(
		&C1ColPos, &sCol1Velo,
		&C2ColPos, &sCol2Velo,
		sCol1Mass, sCol2Mass,
		SPHERE_REPULSION, SPHERE_REPULSION,
		t,
		&C1ColPos, &C1Velo,
		&C2ColPos, &C2Velo))
		return;

	// 衝突後位置に移動
	sCol1Velo = C1Velo;
	sCol2Velo = C2Velo;
	sCol1Pos += C1Velo;
	sCol2Pos += C2Velo;
	
	if (sCol1->GetGameObject()->pParent != nullptr)
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&sCol1->GetGameObject()->pParent->worldMat);
		ConvertToLocalSpace(sCol1Pos, parentWorld);
		ConvertToLocalSpace(sCol1PrePos, parentWorld);
	}

	if (sCol2->GetGameObject()->pParent != nullptr)
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&sCol2->GetGameObject()->pParent->worldMat);
		ConvertToLocalSpace(sCol2Pos, parentWorld);
		ConvertToLocalSpace(sCol2PrePos, parentWorld);
	}

	XMStoreFloat3(&sCol1->GetGameObject()->pos, sCol1Pos);
	XMStoreFloat3(&sCol2->GetGameObject()->pos, sCol2Pos);
	XMStoreFloat3(&sCol1->GetPhysics()->prePos, sCol1PrePos);
	XMStoreFloat3(&sCol2->GetPhysics()->prePos, sCol2PrePos);
	XMStoreFloat3(&sCol1->GetPhysics()->velocity, sCol1Velo);
	XMStoreFloat3(&sCol2->GetPhysics()->velocity, sCol2Velo);

	sCol1->GetPhysics()->OnCollisionEnter(sCol2->GetGameObject());
	sCol2->GetPhysics()->OnCollisionEnter(sCol1->GetGameObject());
}

void CollisionOfMeshAndSphere(MeshCollider* mCol, SphereCollider* sCol)
{
	//球とメッシュの当たり判定
	vector<NODE_COLLISION> nodeCol = mCol->GetMesh()->GetMeshData();

	XMVECTOR sColPrePos = XMLoadFloat3(&sCol->GetPhysics()->prePos);
	XMVECTOR sColPos = XMLoadFloat3(&sCol->GetGameObject()->pos);
	if (sCol->GetGameObject()->pParent != nullptr)
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&sCol->GetGameObject()->pParent->worldMat);
		ConvertToParentSpace(sColPos, parentWorld);
		ConvertToParentSpace(sColPrePos, parentWorld);
	}

	XMVECTOR RefV;	// 反射後の速度ベクトル
	XMVECTOR sColVelo = XMLoadFloat3(&sCol->GetPhysics()->velocity);
	XMVECTOR ColPos;
	float t = 0;
	bool hit = false;

	for (int i = 0; i < nodeCol.size(); i++)
	{
		if (nodeCol[i].m_polyDataArray.size() == 0)
			continue;

		for (int j = 0; j < nodeCol[i].m_polyDataArray.size(); j++)
		{
			XMVECTOR normal = XMLoadFloat3(&nodeCol[i].m_polyDataArray[j].normal);
			XMVECTOR pos[3] =
			{
				XMLoadFloat3(&nodeCol[i].m_polyDataArray[j].vertexArray[0].vPos),
				XMLoadFloat3(&nodeCol[i].m_polyDataArray[j].vertexArray[1].vPos),
				XMLoadFloat3(&nodeCol[i].m_polyDataArray[j].vertexArray[2].vPos)
			};

			// 衝突している円とメッシュの衝突位置を検出
			if (!Collision::SpherePolygonCollision(
				sCol->radius,
				&sColPrePos, &sColPos,
				&normal, pos,
				&t,
				&ColPos))

			{
				continue;
			}
			else
			{
				// 反射後の速度ベクトルを取得
				GetRefrectVelo(&RefV, normal, sColVelo, WALL_REPULSION);

				//t = 1.0f - t;

				// 反射後の位置に移動
				GetRelectedPos(t, *sCol, RefV);

				hit = true;
			}
		}
	}


	if (hit)
	{
		mCol->GetPhysics()->OnCollisionEnter(sCol->GetGameObject());
		sCol->GetPhysics()->OnCollisionEnter(mCol->GetGameObject());
	}
}


//システムへの登録
inline void Collider::_Register_(Collider* pCol)
{
	//先頭が空の場合は新規タスクを設定
	if (pBegin == nullptr)
	{
		pBegin = pCol;
	}
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
	if (next != nullptr) 
		next->pPrev = prev;
	else
		pEnd = prev;

	//前に次を代入してつなぎ合わせる
	if (prev != nullptr)
		prev->pNext = next;
	else 
		pBegin = next;

	//タスクの消去
	SAFE_DELETE(pCol);

	//次のタスクを返す
	return next;
}

Collider::Collider() : pPrev(nullptr), pNext(nullptr), pObject(nullptr), pPhysics(nullptr)
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

PhysicsComponent * Collider::GetPhysics()
{
	return pPhysics;
}

void Collider::AddTargetTag(const string & t)
{
	auto it = find(targetTagList.begin(), targetTagList.end(), t);

	if (it != targetTagList.end())
		return;

	targetTagList.push_back(t);
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

	Collider* pCol = pBegin; //現在のタスク

	//末尾までループする
	while (pCol != nullptr)
	{
		Collider* next = pCol->pNext;
		
		pCol->pObjectTree->Remove();

		pCol = next;
	}
}


//////////////////////////////////////////////////////
// class SphereCollider
//////////////////////////////////////////////////////
void SphereCollider::Create(GameObject * pObj, ColliderType type, float r)
{
	Collider::_Register_(this);
	pObject = pObj;
	colType = type;
	radius = r;

	pObjectTree = NEW ObjectTree();
	pObjectTree->pCol = this;

	pPhysics = GetComponent<PhysicsComponent>(pObject);
}

void SphereCollider::Update()
{
	if (pObject->isActive())
	{
		// 再登録
		XMFLOAT3 tmpMin(pObject->pos.x - radius, pObject->pos.y - radius, pObject->pos.z - radius);
		XMFLOAT3 tmpMax(pObject->pos.x + radius, pObject->pos.y + radius, pObject->pos.z + radius);
		SpaceDivision::Instance().Regist(&tmpMin, &tmpMax, pObjectTree);
	}
}

void SphereCollider::CollisionDetection(Collider* pCol)
{
	pCol->isCollision(this);
}

void SphereCollider::isCollision(SphereCollider * other)
{
	CollisionOfSpheres(this, other);
}

void SphereCollider::isCollision(BoxCollider * other)
{
	if (Collision::HitCheckSphereAndAABB(
		pObject->pos,
		radius,
		pPhysics->velocity,
		other->minPos,
		other->maxPos,
		other->GetPhysics()->velocity
	))
	{
		other->GetPhysics()->OnCollisionEnter(pObject);
		pPhysics->OnCollisionEnter(other->GetGameObject());
	}
	else
	{
		return;
	}
}

void SphereCollider::isCollision(MeshCollider * other)
{
	CollisionOfMeshAndSphere(other, this);
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

	pPhysics = GetComponent<PhysicsComponent>(pObject);
}

void BoxCollider::Update()
{
	minPos.x = pObject->pos.x - size;
	minPos.y = pObject->pos.y - size;
	minPos.z = pObject->pos.z - size;
	maxPos.x = pObject->pos.x + size;
	maxPos.y = pObject->pos.y + size;
	maxPos.z = pObject->pos.z + size;

	if (pObject->isActive())
	{
		// 再登録
		SpaceDivision::Instance().Regist(&minPos, &maxPos, pObjectTree);
	}
}

void BoxCollider::CollisionDetection(Collider * pCol)
{
	pCol->isCollision(this);
}

void BoxCollider::isCollision(SphereCollider * other)
{
	if (Collision::HitCheckSphereAndAABB(
		other->GetGameObject()->pos,
		other->radius,
		other->GetPhysics()->velocity,
		minPos,
		maxPos,
		GetPhysics()->velocity
	))
	{
		other->GetPhysics()->OnCollisionEnter(pObject);
		GetPhysics()->OnCollisionEnter(other->GetGameObject());
	}
	else
	{
		return;
	}
}

void BoxCollider::isCollision(BoxCollider * other)
{
	//TODO::衝突判定追加する
}

void BoxCollider::isCollision(MeshCollider * other)
{
	//TODO::衝突判定追加する
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
	pMeshCol->SetMatrix(pObject->worldMat);

	pObjectTree = NEW ObjectTree();
	pObjectTree->pCol = this;

	pPhysics = GetComponent<PhysicsComponent>(pObject);
}

void MeshCollider::Update()
{
	if (pObject->isActive())
	{
		pMeshCol->SetMatrix(pObject->worldMat);
		pMeshCol->Update();

		// 再登録
		XMFLOAT3 tmpMin(pObject->pos.x - size, pObject->pos.y - size, pObject->pos.z - size);
		XMFLOAT3 tmpMax(pObject->pos.x + size, pObject->pos.y + size, pObject->pos.z + size);
		SpaceDivision::Instance().Regist(&tmpMin, &tmpMax, pObjectTree);
	}
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
	CollisionOfMeshAndSphere(this, other);
}

void MeshCollider::isCollision(BoxCollider * other)
{
	//TODO::衝突判定追加する
}

void MeshCollider::isCollision(MeshCollider * other)
{
	//TODO::衝突判定追加する
}