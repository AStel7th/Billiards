#include "../Header/Collider.h"
#include "../Header/GameObject.h"
#include "../Header/Component.h"
#include "../Header/SpaceDivision.h"
#include "../Header/func.h"
#include "../Header/Collision.h"
#include "../Header/CollisionFromFBX.h"




// �ǂƋ��̔��˃x�N�g�����v�Z
// GetRefrectVelo(���˃x�N�g��,�ǂ̖@��,�ړ��x�N�g��,�ǂ̔����W��)
void GetRefrectVelo(XMFLOAT3 *pOut, XMFLOAT3 &N, XMFLOAT3 &V, float e)
{
	XMVECTOR valN = XMLoadFloat3(&N);
	XMVECTOR valV = XMLoadFloat3(&V);
	valN = XMVector3Normalize(valN);
	XMStoreFloat3(pOut,valV - (1 + e)*XMVector3Dot(valN, valV)*valN);
}


// �ǂƂ̔��ˌ�̈ʒu���Z�o
void GetRelectedPos(float Res_time, Collider &c, XMFLOAT3 &RefV)
{
	PhysicsComponent* pPC = GetComponent<PhysicsComponent>(c.GetGameObject());
	XMVECTOR p = XMLoadFloat3(&c.GetGameObject()->pos);
	XMVECTOR pre_p = XMLoadFloat3(&pPC->prePos);
	XMVECTOR v = XMLoadFloat3(&pPC->velocity);

	// �Փˈʒu
	// 0.99�͕ǂɂ߂荞�܂Ȃ����߂̕␳
	p = pre_p + v * (1 - Res_time)*0.99f;
	// ���˃x�N�g��
	v = XMLoadFloat3(&RefV);
	// �ʒu��␳
	p += v * Res_time;

	XMStoreFloat3(&c.GetGameObject()->pos, p);
}

//
//
//// 2���Փˏ���
//void SphereColProc(Collider* c1, Collider *s2)
//{
//	float t = 0;
//	D3DXVECTOR3 C1ColPos, C2ColPos, C1Velo, C2Velo;
//
//	// �Փ˂��Ă���2�~�̏Փˈʒu�����o
//	if (!CheckParticleCollision(
//		s1->r, s2->r,
//		&s1->Pre_p, &s1->p,
//		&s2->Pre_p, &s2->p,
//		&t,
//		&C1ColPos,
//		&C2ColPos))
//		return;	// �Փ˂��Ă��Ȃ��悤�ł�
//
//				// �Փˈʒu��O�ʒu�Ƃ��ĕۑ�
//	s1->p = C1ColPos;
//	s2->p = C2ColPos;
//	s1->Pre_p = C1ColPos;
//	s2->Pre_p = C2ColPos;
//
//	// �Փˌ�̑��x���Z�o
//	if (!CalcParticleColliAfterPos(
//		&C1ColPos, &s1->v,
//		&C2ColPos, &s2->v,
//		s1->w, s2->w,
//		g_Circle_Ref, g_Circle_Ref,		// ���̔����W��
//		t,
//		&C1ColPos, &C1Velo,
//		&C2ColPos, &C2Velo))
//		return; // �������s�����悤�ł�
//
//				// �Փˌ�ʒu�Ɉړ�
//	s1->v = C1Velo;
//	s2->v = C2Velo;
//	s1->p += s1->v;
//	s2->p += s2->v;
//}

// TODO::�{�[����Physics�R���|�[�l���g�ŊǗ�
//// ���̋��̈ʒu���擾
//void GetNextPos(Collider &c)
//{
//	XMFLOAT3 RefV;	// ���ˌ�̑��x�x�N�g��
//	D3DXVECTOR3 ColliPos;	// �Փˈʒu
//	float Res_time = 0.0f;	// �Փˌ�̈ړ��\����
//
//							// �d�͂��|���ė��Ƃ�
//	c.GetPhysics()->velocity.y -= g_Gravity / 60;	// 1�t���[����9.8/60(m/s)����
//
//								// ���̑��x�ňʒu���X�V
//	c.GetPhysics()->prePos = c.GetGameObject()->pos;		// �O�̈ʒu��ۑ�
//	c.GetGameObject()->pos += s.v;			// �ʒu�X�V
//
//						// �ǂƂ̏Փ˂��`�F�b�N
//						// X����
//	if (s.p.x<s.r && g_XLeft) {
//		// ���ˌ�̑��x�x�N�g�����擾
//		GetRefrectVelo(&RefV, D3DXVECTOR3(1, 0, 0), s.v, Wall_Ref);
//		// �c�莞�ԎZ�o
//		Res_time = (s.p.x - s.r) / s.v.x;
//		// ���ˌ�̈ʒu���Z�o
//		GetRelectedPos(Res_time, s, RefV);
//	}
//	// X�E��
//	else if (s.p.x>640 - s.r && g_XRight) {
//		GetRefrectVelo(&RefV, D3DXVECTOR3(-1, 0, 0), s.v, Wall_Ref);
//		Res_time = (s.p.x - 640 + s.r) / s.v.x;
//		GetRelectedPos(Res_time, s, RefV);
//	}
//	// Z��O��
//	if (s.p.z<s.r && g_ZNear) {
//		GetRefrectVelo(&RefV, D3DXVECTOR3(0, 0, 1), s.v, Wall_Ref);
//		Res_time = (s.p.z - s.r) / s.v.z;
//		GetRelectedPos(Res_time, s, RefV);
//	}
//	// Z����
//	else if (s.p.z>640 - s.r && g_ZFar) {
//		GetRefrectVelo(&RefV, D3DXVECTOR3(0, 0, -1), s.v, Wall_Ref);
//		Res_time = (s.p.z - 640 + s.r) / s.v.z;
//		GetRelectedPos(Res_time, s, RefV);
//	}
//	// Y����
//	else if (s.p.y<s.r) {
//		GetRefrectVelo(&RefV, D3DXVECTOR3(0, 1, 0), s.v, Wall_Ref);
//		Res_time = (s.p.y - s.r) / s.v.y;
//		GetRelectedPos(Res_time, s, RefV);
//	}
//}


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
	////���ƃ��b�V���̓����蔻��
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

	//		// �Փ˂��Ă���~�ƃ��b�V���̏Փˈʒu�����o
	//		if (!Collision::SpherePolygonCollision(
	//			radius,
	//			&myPrePos, &myPos,
	//			&normal, pos,
	//			&t,
	//			&ColPos))
	//			return;	// �Փ˂��Ă��Ȃ��悤�ł�
	//	}
	//}
	//

	//			// �Փˈʒu��O�ʒu�Ƃ��ĕۑ�
	//myPos = ColPos;
	//myPrePos = ColPos;

	//XMVECTOR myVelo = XMLoadFloat3(&myPC->velocity);
	//XMVECTOR otherVelo = XMLoadFloat3(&otherPC->velocity);
	//float myMass = myPC->mass;
	//float otherMass = otherPC->mass;


	//// �Փˌ�̑��x���Z�o
	//if (!Collision::CalcParticleColliAfterPos(
	//	&C1ColPos, &myVelo,
	//	&C2ColPos, &otherVelo,
	//	myMass, otherMass,
	//	SPHERE_REPULSION, SPHERE_REPULSION,		// ���̔����W��
	//	t,
	//	&C1ColPos, &C1Velo,
	//	&C2ColPos, &C2Velo))
	//	return; // �������s�����悤�ł�

	//			// �Փˌ�ʒu�Ɉړ�
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

	pObjectTree = NEW ObjectTree();
	pObjectTree->pCol = this;
}

void MeshCollider::Update()
{
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
