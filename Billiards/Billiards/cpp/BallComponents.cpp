#include "../Header/BallComponents.h"
#include "../Header/GameObject.h"
#include "../Header/DrawSystem.h"
#include "../Header/MeshData.h"
#include "../Header/MaterialData.h"
#include "../Header/InputDeviceManager.h"
#include <string>
#include <tchar.h>

using namespace std;

BallPhysics::BallPhysics(GameObject* pObj) : PhysicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	prePos = pGameObject->pos;
	//velocity.y = -0.1f;
	//velocity.z = -0.1f;
}

BallPhysics::~BallPhysics()
{
}

void BallPhysics::Update()
{
	velocity.y -= GRAVITY / 60.0f;
	//velocity.x -= 0.01f;
	
	prePos = pGameObject->pos;
	pGameObject->pos.x += velocity.x;
	pGameObject->pos.y += velocity.y;
	pGameObject->pos.z += velocity.z;
	//pGameObject->pos.z += velocity.z;
	TCHAR s[256];

	_stprintf_s(s, _T("pos:%f %f    ButtonDown %s\n"), InputDeviceManager::Instance().GetMouseState().x,InputDeviceManager::Instance().GetMouseState().y, InputDeviceManager::Instance().GetMouseButtonDown(0) ? "push" : "none");
	
	OutputDebugString(s);
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

	XMMATRIX _world = XMMatrixIdentity();
	_world *= XMMatrixRotationQuaternion(XMVectorSet(pGameObject->rot.x, pGameObject->rot.y, pGameObject->rot.z, 1.0f));
	_world *= XMMatrixScaling(pGameObject->scale.x, pGameObject->scale.y, pGameObject->scale.z);
	_world *= XMMatrixTranslation(pGameObject->pos.x, pGameObject->pos.y, pGameObject->pos.z);

	XMStoreFloat4x4(&world, _world);

	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);
}

BallGraphics::~BallGraphics()
{
}

void BallGraphics::Update()
{
	XMMATRIX _world = XMMatrixIdentity();
	_world *= XMMatrixRotationQuaternion(XMVectorSet(pGameObject->rot.x, pGameObject->rot.y, pGameObject->rot.z, 1.0f));
	_world *= XMMatrixScaling(pGameObject->scale.x, pGameObject->scale.y, pGameObject->scale.z);
	_world *= XMMatrixTranslation(pGameObject->pos.x, pGameObject->pos.y, pGameObject->pos.z);

	XMStoreFloat4x4(&world, _world);
}
