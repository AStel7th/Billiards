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

// �ǂƋ��̔��˃x�N�g�����v�Z
// GetRefrectVelo(���˃x�N�g��,�ǂ̖@��,�ړ��x�N�g��,�ǂ̔����W��)
void GetRefrectVelo(XMVECTOR *pOut, XMVECTOR &N, XMVECTOR &V, float e)
{
	N = XMVector3Normalize(N);
	*pOut = V - (1 + e)*XMVector3Dot(N, V)*N;
}


// �ǂƂ̔��ˌ�̈ʒu���Z�o
void GetRelectedPos(float Res_time, Collider &c, XMVECTOR& RefV)
{
	XMVECTOR p = XMLoadFloat3(&c.GetGameObject()->pos);
	XMVECTOR pre_p = XMLoadFloat3(&c.GetPhysics()->prePos);
	XMVECTOR v = XMLoadFloat3(&c.GetPhysics()->velocity);

	// �Փˈʒu
	// 0.99�͕ǂɂ߂荞�܂Ȃ����߂̕␳
	p = pre_p + v * (1 - Res_time)*0.99f;
	// ���˃x�N�g��
	v = RefV;
	// �ʒu��␳
	p += v * Res_time;

	XMStoreFloat3(&c.GetGameObject()->pos, p);
	XMStoreFloat3(&c.GetPhysics()->velocity, v);
}

void CollisionOfSpheres(SphereCollider* sCol1, SphereCollider* sCol2 )
{
	//���Ƌ��̓����蔻��
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

	// �Փ˂��Ă���2�~�̏Փˈʒu�����o
	if (!Collision::CheckParticleCollision(
		sCol1->radius, sCol2->radius,
		&sCol1PrePos, &sCol1Pos,
		&sCol2PrePos, &sCol2Pos,
		&t,
		&C1ColPos,
		&C2ColPos))
		return;

	// �Փˈʒu��O�ʒu�Ƃ��ĕۑ�
	sCol1Pos = C1ColPos;
	sCol2Pos = C2ColPos;
	sCol1PrePos = C1ColPos;
	sCol2PrePos = C2ColPos;

	XMVECTOR sCol1Velo = XMLoadFloat3(&sCol1->GetPhysics()->velocity);
	XMVECTOR sCol2Velo = XMLoadFloat3(&sCol2->GetPhysics()->velocity);
	float sCol1Mass = sCol1->GetPhysics()->mass;
	float sCol2Mass = sCol2->GetPhysics()->mass;


	// �Փˌ�̑��x���Z�o
	if (!Collision::CalcParticleColliAfterPos(
		&C1ColPos, &sCol1Velo,
		&C2ColPos, &sCol2Velo,
		sCol1Mass, sCol2Mass,
		SPHERE_REPULSION, SPHERE_REPULSION,
		t,
		&C1ColPos, &C1Velo,
		&C2ColPos, &C2Velo))
		return;

	// �Փˌ�ʒu�Ɉړ�
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
	//���ƃ��b�V���̓����蔻��
	vector<NODE_COLLISION> nodeCol = mCol->GetMesh()->GetMeshData();

	XMVECTOR sColPrePos = XMLoadFloat3(&sCol->GetPhysics()->prePos);
	XMVECTOR sColPos = XMLoadFloat3(&sCol->GetGameObject()->pos);
	if (sCol->GetGameObject()->pParent != nullptr)
	{
		XMMATRIX parentWorld = XMLoadFloat4x4(&sCol->GetGameObject()->pParent->worldMat);
		ConvertToParentSpace(sColPos, parentWorld);
		ConvertToParentSpace(sColPrePos, parentWorld);
	}

	XMVECTOR RefV;	// ���ˌ�̑��x�x�N�g��
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

			// �Փ˂��Ă���~�ƃ��b�V���̏Փˈʒu�����o
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
				// ���ˌ�̑��x�x�N�g�����擾
				GetRefrectVelo(&RefV, normal, sColVelo, WALL_REPULSION);

				//t = 1.0f - t;

				// ���ˌ�̈ʒu�Ɉړ�
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


//�V�X�e���ւ̓o�^
inline void Collider::_Register_(Collider* pCol)
{
	//�擪����̏ꍇ�͐V�K�^�X�N��ݒ�
	if (pBegin == nullptr)
	{
		pBegin = pCol;
	}
	else
	{
		//�V�K�^�X�N�̑O�ɖ����^�X�N����
		pCol->pPrev = pEnd;

		//�����^�X�N�̎��ɐV�K�^�X�N����
		pEnd->pNext = pCol;
	}

	//�����^�X�N���V�K�^�X�N�ɂȂ�
	pEnd = pCol;
}

//�V�X�e����������@���̃|�C���^���Ԃ����
inline Collider* Collider::_Unregister_(Collider* pCol)
{
	Collider* next = pCol->pNext; //���g�̎�
	Collider* prev = pCol->pPrev; //���g�̑O

	//���ʒu�ɑO�ʒu�������ĂȂ����킹��
	if (next != nullptr) 
		next->pPrev = prev;
	else
		pEnd = prev;

	//�O�Ɏ��������ĂȂ����킹��
	if (prev != nullptr)
		prev->pNext = next;
	else 
		pBegin = next;

	//�^�X�N�̏���
	SAFE_DELETE(pCol);

	//���̃^�X�N��Ԃ�
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

	Collider* pCol = pBegin; //���݂̃^�X�N

	//�����܂Ń��[�v����
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
		// �ēo�^
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
		// �ēo�^
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
	//TODO::�Փ˔���ǉ�����
}

void BoxCollider::isCollision(MeshCollider * other)
{
	//TODO::�Փ˔���ǉ�����
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

		// �ēo�^
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
	//TODO::�Փ˔���ǉ�����
}

void MeshCollider::isCollision(MeshCollider * other)
{
	//TODO::�Փ˔���ǉ�����
}