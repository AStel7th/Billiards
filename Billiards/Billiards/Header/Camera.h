#pragma once
#include <DirectXMath.h>

using namespace DirectX;
//--------------------------------------------------------------------------------------
// Camera
//--------------------------------------------------------------------------------------
class Camera {
protected:
	Camera() :view(), proj() {};
public:
	static Camera &Instance()
	{
		static Camera inst;
		return inst;
	}

	XMFLOAT3 m_eye, m_at, m_up;
	float m_fov, m_aspect, m_zn, m_zf;

	XMFLOAT4X4 view, proj;// ビュープロジェクション
						  // シングルトンオブジェクトを取得

	void SetView(XMFLOAT3& eye = XMFLOAT3(0.0f, 0.0f, -10.0f), XMFLOAT3& at = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3& up = XMFLOAT3(0.0f, 1.0f, 0.0f))
	{
		m_eye = eye;
		m_at = at;
		m_up = up;

		XMStoreFloat4x4(&view, XMMatrixLookAtLH(XMVectorSet(m_eye.x, m_eye.y, m_eye.z, 0), XMVectorSet(m_at.x, m_at.y, m_at.z, 0), XMVectorSet(m_up.x, m_up.y, m_up.z, 0)));
	}

	void SetProj(float fov, float aspect, float zn, float zf)
	{
		m_fov = fov;
		m_aspect = aspect;
		m_zn = zn;
		m_zf = zf;

		XMStoreFloat4x4(&proj, XMMatrixPerspectiveFovLH(m_fov, m_aspect, m_zn, m_zf));
	}
};