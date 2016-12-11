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

BallPhysics::BallPhysics(GameObject* pObj) : PhysicsComponent(), isMove(false)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	WaveFileLoader* data;
	ResourceManager::Instance().GetResource(&data, "BallHit", "Resource/Sound/BallHit.wav");

	hitSE = NEW SoundPlayer();
	hitSE->Create(data);

	mass = 170.0f;		// ビリヤードのボールの重さ

	prePos = pGameObject->pos;
	/*velocity.x = -0.2f;
	velocity.z = -1.5f;*/
}

BallPhysics::~BallPhysics()
{
}

void BallPhysics::Update()
{
	velocity.y -= GRAVITY / 60.0f;

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
}

void BallPhysics::OnCollisionEnter(GameObject * other)
{
	if (other->tag != "Table" && !isMove)
	{
		Messenger::BallMovement(pGameObject, true);
		hitSE->Play();
	}

	if(other->tag == "Pocket")
		Messenger::BallMovement(pGameObject, false);
}


BallGraphics::BallGraphics(GameObject* pObj, MeshData* mesh,int num) : GraphicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pMeshData = mesh;
	MaterialData* matData = MaterialManager::Instance().CreateMaterial("BallTex" + to_string(num));

	for each (MESH var in pMeshData->GetMeshList())
	{
		if (var.materialData.size() == 0)
			continue;

		matData = var.materialData[0];
		matData->SetTexture("Texture/" + to_string(num) + ".png");
		var.materialData[0] = matData;
	}

	/*DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);*/
}

BallGraphics::~BallGraphics()
{
}

void BallGraphics::Update()
{
	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);
}
