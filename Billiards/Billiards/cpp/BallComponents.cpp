#include "../Header/BallComponents.h"
#include "../Header/GameObject.h"
#include "../Header/DrawSystem.h"
#include "../Header/ResourceManager.h"
#include "../Header/AudioSystem.h"
#include "../Header/MeshData.h"
#include "../Header/MaterialData.h"
#include "../Header/Messenger.h"
#include <string>
#include <tchar.h>

using namespace std;


BallPhysics::BallPhysics(GameObject* pObj) : PhysicsComponent(), isMove(false), firstHit(nullptr)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	WaveFileLoader* data;
	ResourceManager::Instance().GetResource(&data, "BallHit", "Resource/Sound/BallHit.wav");

	hitSE = NEW SoundPlayer();
	hitSE->Create(data);

	mass = 17.0f;		// ビリヤードのボールの重さ

	prePos = pGameObject->pos;

	Messenger::OnGamePhase.Add(*this, &BallPhysics::GamePhase);
}

BallPhysics::~BallPhysics()
{
	SAFE_DELETE(hitSE);
}

bool BallPhysics::Update()
{
	velocity.y -= GRAVITY / 45.0f;

	//減速処理
	XMVECTOR _velo = XMLoadFloat3(&velocity);
	_velo *= 0.99f;

	XMStoreFloat3(&velocity, _velo);
	
	if (fabsf(velocity.x) < 0.01f && fabsf(velocity.z) < 0.01f)
	{
		Messenger::BallMovement(pGameObject, false);
		isMove = false;
	}

	prePos = pGameObject->pos;
	pGameObject->pos.x += velocity.x;
	pGameObject->pos.y += velocity.y;
	pGameObject->pos.z += velocity.z;

	pGameObject->SetWorld();

	return true;
}


void BallPhysics::GamePhase(GAME_STATE state)
{
	if (state == GAME_STATE::Shot)
		firstHit = nullptr;
}

void BallPhysics::OnCollisionEnter(GameObject * other)
{
	if(other->tag == "Ball" || other->tag == "Cues")
		hitSE->Play();

	if (pGameObject->name == "HandBall" && other->tag == "Ball" && firstHit == nullptr)
	{
		firstHit = other;
		Messenger::SetFirstHitBall(firstHit);
	}

	if (other->tag != "Table" && !isMove)
	{
		Messenger::BallMovement(pGameObject, true);
	}

	if(other->tag == "Pocket")
		Messenger::BallMovement(pGameObject, false);
}


BallGraphics::BallGraphics(GameObject* pObj, MeshData* mesh,int num) : GraphicsComponent(), pPhysics(nullptr)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pMeshData = mesh;
	MaterialData* matData = MaterialManager::Instance().CreateMaterial("BallTex" + to_string(num));

	//テクスチャ差し替え
	for each (MESH var in pMeshData->GetMeshList())
	{
		if (var.materialData.size() == 0)
			continue;

		matData = var.materialData[0];
		matData->SetTexture("Resource/Texture/" + to_string(num) + ".png");
		var.materialData[0] = matData;
	}
	
	pPhysics = GetComponent<PhysicsComponent>(pGameObject);

	circumference  = (2.6f * 2) * XM_PI;		//円周の長さ
}

BallGraphics::~BallGraphics()
{
}

bool BallGraphics::Update()
{
	//回転処理
	float ang;
	XMMATRIX rot;
	XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR dirVec = XMLoadFloat3(&XMFLOAT3(pPhysics->velocity.x,0.0f, pPhysics->velocity.z));
	float length;
	XMStoreFloat(&length, XMVector3Length(dirVec));
	dirVec = XMVector3Normalize(dirVec);

	XMVECTOR cross = XMVector3Cross(upVec, dirVec);

	if (length > 0.01f)
	{
		float proportion = length / circumference;

		ang = 360.0f * proportion;
	}
	else
	{
		ang = 0.0f;
	}
	
	rot = XMMatrixIdentity();

	if (!XMVector3Equal(cross, XMVectorZero()))
	{
		rot = XMMatrixRotationAxis(cross, RADIAN(ang));
	}
	
	//行列からオイラー角取得
	XMFLOAT3 eulerRot = MatrixToEulerXYZ(rot);

	pGameObject->rot.x += eulerRot.x;
	pGameObject->rot.y += eulerRot.y;
	pGameObject->rot.z += eulerRot.z;

	pGameObject->SetWorld();

	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);

	return true;
}
