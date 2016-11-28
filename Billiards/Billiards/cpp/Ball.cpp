#include "../Header/Ball.h"
#include "../Header/ResourceManager.h"
#include "../Header/Collider.h"
#include "../Header/BallComponents.h"

Ball::Ball(int num,float x, float y, float z) : GameObject(), pMesh(nullptr), pCollider(nullptr), pPhysicsComponent(nullptr), pGraphicsComponent(nullptr)
{
	SetTag("Ball" + to_string(num));

	pMesh = ResourceManager::Instance().GetResource("Ball" + to_string(num), "Resource/Ball.fbx");

	pPhysicsComponent = NEW BallPhysics(this);

	pGraphicsComponent = NEW BallGraphics(this, pMesh,num);

	pCollider = NEW SphereCollider();
	pCollider->Create(this, Sphere, 2.6f);

	pos.x = x;
	pos.y = y;
	pos.z = z;

	XMMATRIX wMat = XMMatrixIdentity();
	wMat *= XMMatrixRotationQuaternion(XMVectorSet(rot.x, rot.y, rot.z, 1.0f));
	wMat *= XMMatrixTranslation(pos.x, pos.y, pos.z);

	XMStoreFloat4x4(&world, wMat);
}

Ball::~Ball()
{
	pCollider->Destroy();
	SAFE_DELETE(pPhysicsComponent);
	SAFE_DELETE(pGraphicsComponent);
}

void Ball::Update()
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
