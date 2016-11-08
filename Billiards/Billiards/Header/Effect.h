//#pragma once
//#include "Effekseer.h"
//#include "EffekseerRendererDX11.h"
//#include "../Header/Direct3D11.h"
//#include "../Header/Camera.h"
//
//#define EFFECT_MAX 2000		//	�ő�G�t�F�N�g��
//
//class EffectManager
//{
//	Direct3D11 &d3d11 = Direct3D11::Instance();
//	Effekseer::Vector3D	position;
//protected:
//	EffectManager();
//public:
//	virtual ~EffectManager();
//
//	// �V���O���g���I�u�W�F�N�g���擾
//	static EffectManager &Instance()
//	{
//		static EffectManager inst;
//		return inst;
//	}
//
//	Effekseer::Manager*	manager = nullptr;
//	EffekseerRenderer::Renderer* renderer = nullptr;
//
//	void Update();
//	void Draw();
//	
//	void SetView(XMFLOAT3* eye, XMFLOAT3*at, XMFLOAT3* up);
//	void SetProj(float fov, float aspect, float zn, float zf);
//};
//
//class Effect
//{
//private:
//	EffectManager &effectManager = EffectManager::Instance();
//
//	Effekseer::Handle handle = -1;
//	Effekseer::Effect* effect = nullptr;
//	Effekseer::Vector3D	position;
//public:
//	Effect();
//	~Effect();
//
//	void Create(const WCHAR* filename);
//
//	void Play();	//�Đ�
//	void Stop();	//��~
//
//	XMFLOAT3& GetLocation();
//	void SetLocation(XMFLOAT3* location);
//	void AddLocation(XMFLOAT3* location);	//�ړ��p
//	void SetRotation(XMFLOAT3* axis, float rad);
//	void SetScale(float x, float y, float z);
//};