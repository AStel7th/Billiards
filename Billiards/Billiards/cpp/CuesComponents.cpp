#include "../Header/CuesComponents.h"
#include "../Header/GameObject.h"
#include "../Header/DrawSystem.h"
#include "../Header/MeshData.h"
#include "../Header/InputDeviceManager.h"

CuesInput::CuesInput(GameObject * pObj) : InputComponent() , whiteBall(nullptr)
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	whiteBall = GameObject::All::GameObjectFindWithTag("Ball0");

	pGameObject->pos = whiteBall->pos;
}

CuesInput::~CuesInput()
{
}

void CuesInput::Update()
{
}

CuesPhysics::CuesPhysics(GameObject * pObj) : PhysicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	prePos = pGameObject->pos;
}

CuesPhysics::~CuesPhysics()
{
}

void CuesPhysics::Update()
{
	prePos = pGameObject->pos;
}

CuesGraphics::CuesGraphics(GameObject * pObj, MeshData * mesh) : GraphicsComponent()
{
	id.push_back(typeid(this));
	pGameObject = pObj;
	pGameObject->AddComponent(this);

	pMeshData = mesh;

	XMMATRIX _world = XMMatrixIdentity();
	_world *= XMMatrixRotationQuaternion(XMVectorSet(pGameObject->rot.x, pGameObject->rot.y, pGameObject->rot.z, 1.0f));
	_world *= XMMatrixScaling(pGameObject->scale.x, pGameObject->scale.y, pGameObject->scale.z);
	_world *= XMMatrixTranslation(pGameObject->pos.x, pGameObject->pos.y, pGameObject->pos.z);

	XMStoreFloat4x4(&world, _world);

	DrawSystem::Instance().AddDrawList(DRAW_PRIOLITY::Opaque, pMeshData->GetName(), this);
}

CuesGraphics::~CuesGraphics()
{
}

void CuesGraphics::Update()
{
	XMMATRIX _world = XMMatrixIdentity();
	_world *= XMMatrixRotationQuaternion(XMVectorSet(pGameObject->rot.x, pGameObject->rot.y, pGameObject->rot.z, 1.0f));
	_world *= XMMatrixScaling(pGameObject->scale.x, pGameObject->scale.y, pGameObject->scale.z);
	_world *= XMMatrixTranslation(pGameObject->pos.x, pGameObject->pos.y, pGameObject->pos.z);

	XMStoreFloat4x4(&world, _world);
}
