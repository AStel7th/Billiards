#include "../Header/Collider.h"
#include "../Header/GameObject.h"
#include "../Header/Component.h"
#include "../Header/SpaceDivision.h"
#include "../Header/func.h"
#include "../Header/Collision.h"
#include "../Header/CollisionFromFBX.h"

#include <tchar.h>




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
	PhysicsComponent* pPC = GetComponent<PhysicsComponent>(c.GetGameObject());
	XMVECTOR p = XMLoadFloat3(&c.GetGameObject()->pos);
	XMVECTOR pre_p = XMLoadFloat3(&pPC->prePos);
	XMVECTOR v = XMLoadFloat3(&pPC->velocity);

	// �Փˈʒu
	// 0.99�͕ǂɂ߂荞�܂Ȃ����߂̕␳
	p = pre_p + v * (1 - Res_time)*0.99f;
	// ���˃x�N�g��
	v = RefV;
	// �ʒu��␳
	p += v * Res_time;

	XMStoreFloat3(&c.GetGameObject()->pos, p);
	XMStoreFloat3(&pPC->velocity, v);
}


//�V�X�e���ւ̓o�^
inline void Collider::_Register_(Collider* pCol)
{
	//�擪����̏ꍇ�͐V�K�^�X�N��ݒ�
	if (pBegin == nullptr) pBegin = pCol;
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
	if (next != nullptr) next->pPrev = prev;
	//null �̏ꍇ�͖����^�X�N���Ȃ��Ȃ����̂ŁA�O�̃^�X�N�𖖔��ɂ���
	else pEnd = prev;

	//�O�Ɏ��������ĂȂ����킹��
	if (prev != nullptr) prev->pNext = next;
	//null �̏ꍇ�͐擪�^�X�N���Ȃ��Ȃ����̂ŁA���̃^�X�N��擪�ɂ���
	else pBegin = next;

	//�^�X�N�̏���
	SAFE_DELETE(pCol);

	//���̃^�X�N��Ԃ�
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

	TCHAR s[256];

	_stprintf_s(s, _T("colCount %d\n"), colVect.size());

	OutputDebugString(s);
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
	pObjectTree->Remove();		// ��x���X�g����O���
	
	// �ēo�^
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
	//���Ƌ��̓����蔻��
	float t = 0;
	XMVECTOR C1ColPos, C2ColPos, C1Velo, C2Velo;

	PhysicsComponent* myPC = GetComponent<PhysicsComponent>(pObject);
	PhysicsComponent* otherPC = GetComponent<PhysicsComponent>(other->GetGameObject());

	XMVECTOR myPrePos = XMLoadFloat3(&myPC->prePos);
	XMVECTOR myPos = XMLoadFloat3(&pObject->pos);
	XMVECTOR otherPrePos = XMLoadFloat3(&otherPC->prePos);
	XMVECTOR otherPos = XMLoadFloat3(&other->GetGameObject()->pos);

	// �Փ˂��Ă���2�~�̏Փˈʒu�����o
	if (!Collision::CheckParticleCollision(
		radius, other->radius,
		&myPrePos, &myPos,
		&otherPrePos, &otherPos,
		&t,
		&C1ColPos,
		&C2ColPos))
		return;	// �Փ˂��Ă��Ȃ��悤�ł�

				// �Փˈʒu��O�ʒu�Ƃ��ĕۑ�
	myPos = C1ColPos;
	otherPos = C2ColPos;
	myPrePos = C1ColPos;
	otherPrePos = C2ColPos;

	XMVECTOR myVelo = XMLoadFloat3(&myPC->velocity);
	XMVECTOR otherVelo = XMLoadFloat3(&otherPC->velocity);
	float myMass = myPC->mass;
	float otherMass = otherPC->mass;


	// �Փˌ�̑��x���Z�o
	if (!Collision::CalcParticleColliAfterPos(
		&C1ColPos, &myVelo,
		&C2ColPos, &otherVelo,
		myMass, otherMass,
		SPHERE_REPULSION, SPHERE_REPULSION,		// ���̔����W��
		t,
		&C1ColPos, &C1Velo,
		&C2ColPos, &C2Velo))
		return; // �������s�����悤�ł�

	// �Փˌ�ʒu�Ɉړ�
	myVelo = C1Velo;
	otherVelo = C2Velo;
	myPos += C1Velo;
	otherPos += C2Velo;

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
	//���ƃ��b�V���̓����蔻��
	vector<NODE_COLLISION> nodeCol = other->GetMesh()->GetMeshData();

	PhysicsComponent* myPC = GetComponent<PhysicsComponent>(pObject);

	XMVECTOR myPrePos = XMLoadFloat3(&myPC->prePos);
	XMVECTOR myPos = XMLoadFloat3(&pObject->pos);
	XMVECTOR RefV;											// ���ˌ�̑��x�x�N�g��
	XMVECTOR otherVelo = XMLoadFloat3(&myPC->velocity);
	XMVECTOR ColPos;
	float t = 0;

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
				radius,
				&myPrePos, &myPos,
				&normal, pos,
				&t,
				&ColPos))
				continue;	// �Փ˂��Ă��Ȃ��悤�ł�
			else
			{
				// ���ˌ�̑��x�x�N�g�����擾
				GetRefrectVelo(&RefV, normal, otherVelo, WALL_REPULSION);

				// ���ˌ�̈ʒu�Ɉړ�
				GetRelectedPos(t, *other, RefV);
			}
		}
	}
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
	pObjectTree->Remove();		// ��x���X�g����O���

								// �ēo�^
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
	pMeshCol->SetMatrix(pObject->world);

	pObjectTree = NEW ObjectTree();
	pObjectTree->pCol = this;
}

void MeshCollider::Update()
{
	pMeshCol->SetMatrix(pObject->world);
	pMeshCol->Update();

	pObjectTree->Remove();		// ��x���X�g����O���

								// �ēo�^
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
	//���ƃ��b�V���̓����蔻��
	vector<NODE_COLLISION> nodeCol = GetMesh()->GetMeshData();

	PhysicsComponent* otherPC = GetComponent<PhysicsComponent>(other->GetGameObject());

	XMVECTOR otherPrePos = XMLoadFloat3(&otherPC->prePos);
	XMVECTOR otherPos = XMLoadFloat3(&other->GetGameObject()->pos);
	XMVECTOR RefV;	// ���ˌ�̑��x�x�N�g��
	XMVECTOR otherVelo = XMLoadFloat3(&otherPC->velocity);
	XMVECTOR ColPos;
	float t = 0;

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
				other->radius,
				&otherPrePos, &otherPos,
				&normal, pos,
				&t,
				&ColPos))
				continue;	// �Փ˂��Ă��Ȃ��悤�ł�
			else
			{
				// ���ˌ�̑��x�x�N�g�����擾
				GetRefrectVelo(&RefV, normal,otherVelo, WALL_REPULSION);

				// ���ˌ�̈ʒu�Ɉړ�
				GetRelectedPos(t, *other, RefV);
			}
		}
	}
}

void MeshCollider::isCollision(BoxCollider * other)
{
	int a = 0;
}

void MeshCollider::isCollision(MeshCollider * other)
{
	int a = 0;
}
