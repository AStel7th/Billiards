#ifndef COLLISION_H
#define COLLISION_H

#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

#define EPSIRON 0.00001f	// �덷

class Collision
{

public:

	//AABB�\����
	typedef struct _AABB
	{
		XMFLOAT3 min;	//BOX�̍���
		XMFLOAT3 max;
		XMFLOAT3 center;
		XMFLOAT3 size;
	}AABB;

	//���\����
	typedef struct _SPHERE
	{
		XMFLOAT3 center;
		FLOAT radius;
	}SPHERE;

	//AABB���m�̂����蔻��
	static bool HitCheckAABB(const AABB& rcAABB1, const XMVECTOR& v,     // obj1�̏��
		const AABB& rcAABB2, const XMVECTOR& v1)  // obj2�̏��
	{ 
		// ���Α��x���o��
		XMVECTOR rv = v - v1;

		//XMFLOAT3�^�ɕϊ�
		XMFLOAT3 rvf;
		XMStoreFloat3(&rvf, rv);

		// obj1��_�Ƃ��Ĉ����Aobj2���g������
		XMFLOAT3 P0 = rcAABB1.min;
		AABB exAABB2;
		exAABB2.min = XMFLOAT3(rcAABB2.min.x - rcAABB1.size.x, rcAABB2.min.y - rcAABB1.size.y, rcAABB2.min.z - rcAABB1.size.z);
		exAABB2.size = XMFLOAT3(rcAABB2.size.x + rcAABB1.size.x, rcAABB2.size.y + rcAABB1.size.y, rcAABB2.size.z + rcAABB1.size.z);

		if (rvf.x != 0)
		{
			FLOAT fLineX = (rvf.x > 0) ? exAABB2.min.x : exAABB2.min.x + exAABB2.size.x;
			FLOAT t = fLineX - (P0.x + rvf.x) / rvf.x;

			if ((t >= 0) && (t <= 1.0f))
			{
				// �Փ˓_(������)��AABB�̐����Ɏ��܂��Ă���ΏՓ�
				FLOAT hitY = P0.y + t * rvf.y;
				if ((hitY >= exAABB2.min.y) && (hitY <= exAABB2.min.y + exAABB2.size.y))
				{
					FLOAT hitZ = P0.z + t * rvf.z;
					if ((hitZ >= exAABB2.min.z) && (hitZ <= exAABB2.min.z + exAABB2.size.z))
					{
						return true;
					}
						
				}
			}
		}

		if (rvf.y != 0)
		{
			FLOAT fLineY = (rvf.y > 0) ? exAABB2.min.y : exAABB2.min.y + exAABB2.size.y;
			FLOAT t = fLineY - (P0.y + rvf.y) / rvf.y;

			if ((t >= 0) && (t <= 1.0f))
			{
				// �Փ˓_(x����)��AABB�̐����Ɏ��܂��Ă���ΏՓ�
				FLOAT hitX = P0.x + t * rvf.x;
				if ((hitX >= exAABB2.min.x) && (hitX <= exAABB2.min.x + exAABB2.size.x))
				{
					FLOAT hitZ = P0.z + t * rvf.z;
					if ((hitZ >= exAABB2.min.z) && (hitZ <= exAABB2.min.z + exAABB2.size.z))
					{
						return true;
					}
				}
			}
		}

		if (rvf.z != 0)
		{
			FLOAT fLineZ = (rvf.z > 0) ? exAABB2.min.z : exAABB2.min.z + exAABB2.size.z;
			FLOAT t = fLineZ - (P0.z + rvf.z) / rvf.z;

			if ((t >= 0) && (t <= 1.0f))
			{
				// �Փ˓_(x����)��AABB�̐����Ɏ��܂��Ă���ΏՓ�
				FLOAT hitX = P0.x + t * rvf.x;
				if ((hitX >= exAABB2.min.x) && (hitX <= exAABB2.min.x + exAABB2.size.x))
				{
					FLOAT hitY = P0.y + t * rvf.y;
					if ((hitY >= exAABB2.min.y) && (hitY <= exAABB2.min.y + exAABB2.size.y))
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	//����AABB
	static bool HitCheckSphereAndAABB(const SPHERE &sphere, const XMFLOAT3& v,     // ���̏��
		const AABB& rcAABB2, const XMVECTOR& v1)	//AABB�̏��
	{
		FLOAT SqLen = 0;   // �����ׂ̂���̒l���i�[
			
		// �e���œ_���ŏ��l�ȉ��������͍ő�l�ȏ�Ȃ�΁A�����l��
		if (sphere.center.x + v.x < rcAABB2.min.x)  // i=0��X�A1��Y�A2��Z�̈Ӗ��ł�
			SqLen += (sphere.center.x - rcAABB2.min.x) * (sphere.center.x - rcAABB2.min.x);
		if (sphere.center.x + v.x > rcAABB2.max.x)
			SqLen += (sphere.center.x - rcAABB2.max.x) * (sphere.center.x - rcAABB2.max.x);
		if (sphere.center.y + v.y < rcAABB2.min.y)
			SqLen += (sphere.center.y - rcAABB2.min.y) * (sphere.center.y - rcAABB2.min.y);
		if (sphere.center.y + v.y > rcAABB2.max.y)
			SqLen += (sphere.center.y - rcAABB2.max.y) * (sphere.center.y - rcAABB2.max.y);
		if (sphere.center.z + v.z < rcAABB2.min.z)
			SqLen += (sphere.center.z - rcAABB2.min.z) * (sphere.center.z - rcAABB2.min.z);
		if (sphere.center.z + v.z > rcAABB2.max.z)
			SqLen += (sphere.center.z - rcAABB2.max.z) * (sphere.center.z - rcAABB2.max.z);
			
		FLOAT dis =	sqrt(SqLen);

		if (dis <= sphere.radius)
		{
			return true;
		}
			
		return false;
	}

	///////////////////////////////////////////////////
	// ���ʃp�[�e�B�N���Փ˔���E�����E�ʒu�Z�o�֐�
	// �߂�l : �Փ�(true), ��Փ�(false)
	//static bool HitCheckSphereAndPlane(
	//	FLOAT �p�[�e�B�N���̔��a,
	//	XMVECTOR *�p�[�e�B�N���̑O�̈ʒu, XMVECTOR *�p�[�e�B�N���̎��̓��B�ʒu,
	//	XMVECTOR *���ʂ̖@��, XMVECTOR *���ʏ��1�_,
	//	FLOAT *�Փˎ��Ԃ��i�[����FLOAT�^�ւ̃|�C���^,
	//	XMVECTOR *�p�[�e�B�N���̏Փˈʒu���i�[����D3DXVECTOR�^�ւ̃|�C���^
	//)
	static bool HitCheckSphereAndPlane(
		FLOAT r,
		XMVECTOR *pPre_pos, XMVECTOR *pPos,
		XMVECTOR *pNormal, XMVECTOR *pPlane_pos,
		FLOAT *t,
		XMVECTOR *pOut_colli
		)
	{
		XMVECTOR C0 = *pPre_pos - *pPlane_pos; // ���ʏ�̈�_���猻�݈ʒu�ւ̃x�N�g��
		XMVECTOR D = *pPos - *pPre_pos; // ���݈ʒu����\��ʒu�܂ł̃x�N�g��
		XMVECTOR N = XMVector3Normalize(*pNormal); // �@����W����


		// ���ʂƒ��S�_�̋������Z�o
		FLOAT Dot_C0;
		XMStoreFloat(&Dot_C0, XMVector3Dot(C0, N));

		FLOAT dist_plane_to_point = fabs(Dot_C0);

		// �i�s�����Ɩ@���̊֌W���`�F�b�N
		FLOAT Dot;
		XMStoreFloat(&Dot, XMVector3Dot(D, N));

		// ���ʂƕ��s�Ɉړ����Ă߂荞��ł���X�y�V�����P�[�X
		if ((0.00001f - fabs(Dot) > 0.0f) && (dist_plane_to_point < r)){
			// �ꐶ�����o���Ȃ��̂ōő厞����Ԃ�
			*t = FLT_MAX;
			// �Փˈʒu�͎d���Ȃ��̂ō��̈ʒu���w��
			*pOut_colli = *pPre_pos;
			return true;
		}

		// �������Ԃ̎Z�o
		*t = (r - Dot_C0) / Dot;

		// �Փˈʒu�̎Z�o
		*pOut_colli = *pPre_pos + (*t) * D;

		// �߂荞��ł�����Փ˂Ƃ��ď����I��
		if (dist_plane_to_point < r)
			return true;

		// �ǂɑ΂��Ĉړ����t�����Ȃ�Փ˂��Ă��Ȃ�
		if (Dot >= 0)
			return false;

		// ���Ԃ�0�`1�̊Ԃɂ���ΏՓ�
		if ((0 <= *t) && (*t <= 1))
			return true;

		return false;
	}

	static bool SpherePolygonCollision(
		FLOAT r,
		XMVECTOR *pPre_pos, XMVECTOR *pPos,
		XMVECTOR *pNormal, XMVECTOR *pPlane_pos,
		FLOAT *t,
		XMVECTOR *pOut_colli
	)
	{
		XMVECTOR C0 = *pPre_pos - *pPlane_pos; // ���ʏ�̈�_���猻�݈ʒu�ւ̃x�N�g��
		XMVECTOR D = *pPos - *pPre_pos; // ���݈ʒu����\��ʒu�܂ł̃x�N�g��
		XMVECTOR N = XMVector3Normalize(*pNormal); // �@����W����


		// ���ʂƒ��S�_�̋������Z�o
		FLOAT Dot_C0;
		XMStoreFloat(&Dot_C0, XMVector3Dot(C0, N));

		FLOAT dist_plane_to_point = fabs(Dot_C0);

		// �i�s�����Ɩ@���̊֌W���`�F�b�N
		FLOAT Dot;
		XMStoreFloat(&Dot, XMVector3Dot(D, N));

		// ���ʂƕ��s�Ɉړ����Ă߂荞��ł���X�y�V�����P�[�X
		if ((EPSIRON - fabs(Dot) > 0.0f) && (dist_plane_to_point < r)) {
			// �ꐶ�����o���Ȃ��̂ōő厞����Ԃ�
			*t = FLT_MAX;
			// �Փˈʒu�͎d���Ȃ��̂ō��̈ʒu���w��
			*pOut_colli = *pPre_pos;
			return true;
		}

		// �������Ԃ̎Z�o
		*t = (r - Dot_C0) / Dot;

		// �Փˈʒu�̎Z�o
		*pOut_colli = *pPre_pos + (*t) * D;

		// �߂荞��ł�����Փ˂Ƃ��ď����I��
		if (dist_plane_to_point < r)
			return true;

		// �ǂɑ΂��Ĉړ����t�����Ȃ�Փ˂��Ă��Ȃ�
		if (Dot >= 0)
			return false;

		// ���Ԃ�0�`1�̊Ԃɂ���ΏՓ�
		if ((0 <= *t) && (*t <= 1))
			return true;

		return false;
	}
};

#endif 