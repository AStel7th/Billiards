#include "../Header/Cues.h"
#include "../Header/ResourceManager.h"
#include "../Header/Collider.h"
#include "../Header/CuesComponents.h"

Cues::Cues() : GameObject(),pMesh(nullptr),pCollider(nullptr),pInputComponent(nullptr),pPhysicsComponent(nullptr),pGraphicsComponent(nullptr)
{
	pMesh = ResourceManager::Instance().GetResource("Cues", "Resource/Cues.fbx");

	pInputComponent = NEW CuesInput(this);

	pPhysicsComponent = NEW CuesPhysics(this);

	pGraphicsComponent = NEW CuesGraphics(this, pMesh);

	pCollider = NEW SphereCollider();
	pCollider->Create(this, Sphere, 0.5f);

	XMMATRIX wMat = XMMatrixIdentity();
	wMat *= XMMatrixRotationQuaternion(XMVectorSet(rot.x, rot.y, rot.z, 1.0f));
	wMat *= XMMatrixScaling(scale.x, scale.y, scale.z);
	wMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);

	XMStoreFloat4x4(&world, wMat);
}

Cues::~Cues()
{
	pCollider->Destroy();
	SAFE_DELETE(pInputComponent);
	SAFE_DELETE(pPhysicsComponent);
	SAFE_DELETE(pGraphicsComponent);
}

void Cues::Update()
{
	pPhysicsComponent->Update();

	pGraphicsComponent->Update();

	XMMATRIX wMat = XMMatrixIdentity();
	wMat *= XMMatrixRotationQuaternion(XMVectorSet(rot.x, rot.y, rot.z, 1.0f));
	wMat *= XMMatrixScaling(scale.x, scale.y, scale.z);
	wMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);

	XMStoreFloat4x4(&world, wMat);

	pCollider->Update();
}