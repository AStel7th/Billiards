#pragma once
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

const float EPSIRON = 0.00001f;	// �덷

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


	///////////////////////////////////////////////////
	// �p�[�e�B�N���Փ˔���E�����E�ʒu�Z�o�֐�
	//   rA          : �p�[�e�B�N��A�̔��a
	//   rB          : �p�[�e�B�N��B�̔��a
	//   pre_pos_A   : �p�[�e�B�N��A�̑O�̈ʒu
	//   pos_A       : �p�[�e�B�N��A�̎��̓��B�ʒu
	//   pre_pos_B   : �p�[�e�B�N��B�̑O�ʒu
	//   pos_B       : �p�[�e�B�N��B�̎��̓��B�ʒu
	//   pout_t      : �Փˎ��Ԃ��i�[����FLOAT�^�ւ̃|�C���^
	//   pout_colli_A : �p�[�e�B�N��A�̏Փˈʒu���i�[����D3DXVECTOR�^�ւ̃|�C���^
	//   pout_colli_B : �p�[�e�B�N��A�̏Փˈʒu���i�[����D3DXVECTOR�^�ւ̃|�C���^

	static bool CheckParticleCollision(
		FLOAT rA, FLOAT rB,
		XMVECTOR *pPre_pos_A, XMVECTOR *pPos_A,
		XMVECTOR *pPre_pos_B, XMVECTOR *pPos_B,
		FLOAT *pOut_t,
		XMVECTOR *pOut_colli_A,
		XMVECTOR *pOut_colli_B
	)
	{
		// �O�ʒu�y�ѓ��B�ʒu�ɂ�����p�[�e�B�N���Ԃ̃x�N�g�����Z�o
		XMVECTOR C0 = *pPre_pos_B - *pPre_pos_A;
		XMVECTOR C1 = *pPos_B - *pPos_A;
		XMVECTOR D = C1 - C0;

		// �Փ˔���p��2���֐��W���̎Z�o
		FLOAT P;
		XMStoreFloat(&P,XMVector3LengthSq(D)); 
		if (P <= 0) 
			return false; // ���������Ɉړ�

		FLOAT Q;
		XMStoreFloat(&Q,XMVector3Dot(C0, D)); 
		if (Q == 0) 
			return false; // ���s

		FLOAT R;
		XMStoreFloat(&R, XMVector3LengthSq(C0));

		// �p�[�e�B�N������
		FLOAT r = rA + rB;

		// �Փ˔��莮
		FLOAT Judge = Q*Q - P*(R - r*r);
		if (Judge < 0) {
			// �Փ˂��Ă��Ȃ�
			return false;
		}

		// �Փˎ��Ԃ̎Z�o
		FLOAT t_plus = (-Q + sqrt(Judge)) / P;
		FLOAT t_minus = (-Q - sqrt(Judge)) / P;

		// �Փˎ��Ԃ�0����1���傫���ꍇ�A�Փ˂��Ȃ�
		//   if( (t_plus < 0 || t_plus > 1) && (t_minus < 0 || t_minus > 1)) return false;
		if (t_minus < 0 || t_minus > 1) return false;

		// �Փˎ��Ԃ̌���it_minus������ɍŏ��̏Փˁj
		*pOut_t = t_minus;

		// �Փˈʒu�̌���
		*pOut_colli_A = *pPre_pos_A + t_minus * (*pPos_A - *pPre_pos_A);
		*pOut_colli_B = *pPre_pos_B + t_minus * (*pPos_B - *pPre_pos_B);

		return true; // �Փ˕�
	}


	///////////////////////////////////////////////////
	// �p�[�e�B�N���Փˌ㑬�x�ʒu�Z�o�֐�
	//   pColliPos_A : �Փ˒��̃p�[�e�B�N��A�̒��S�ʒu
	//   pVelo_A     : �Փ˂̏u�Ԃ̃p�[�e�B�N��A�̑��x
	//   pColliPos_B : �Փ˒��̃p�[�e�B�N��B�̒��S�ʒu
	//   pVelo_B     : �Փ˂̏u�Ԃ̃p�[�e�B�N��B�̑��x
	//   weight_A    : �p�[�e�B�N��A�̎���
	//   weight_B    : �p�[�e�B�N��B�̎���
	//   res_A       : �p�[�e�B�N��A�̔�����
	//   res_B       : �p�[�e�B�N��B�̔�����
	//   time        : ���ˌ�̈ړ��\����
	//   pOut_pos_A  : �p�[�e�B�N��A�̔��ˌ�ʒu
	//   pOut_velo_A : �p�[�e�B�N��A�̔��ˌ㑬�x�x�N�g��
	//   pOut_pos_B  : �p�[�e�B�N��B�̔��ˌ�ʒu
	//   pOut_velo_B : �p�[�e�B�N��B�̔��ˌ㑬�x�x�N�g��
	static bool CalcParticleColliAfterPos(
		XMVECTOR *pColliPos_A, XMVECTOR *pVelo_A,
		XMVECTOR *pColliPos_B, XMVECTOR *pVelo_B,
		FLOAT weight_A, FLOAT weight_B,
		FLOAT res_A, FLOAT res_B,
		FLOAT time,
		XMVECTOR *pOut_pos_A, XMVECTOR *pOut_velo_A,
		XMVECTOR *pOut_pos_B, XMVECTOR *pOut_velo_B
	)
	{
		FLOAT TotalWeight = weight_A + weight_B; // ���ʂ̍��v
		FLOAT RefRate = (1 + res_A*res_B); // ������
		XMVECTOR C = *pColliPos_B - *pColliPos_A; // �Փˎ��x�N�g��
		C = XMVector3Normalize(C);
		FLOAT Dot;
		XMStoreFloat(&Dot,XMVector3Dot((*pVelo_A - *pVelo_B), C)); // ���ώZ�o
		XMVECTOR ConstVec = RefRate*Dot / TotalWeight * C; // �萔�x�N�g��

															  // �Փˌ㑬�x�x�N�g���̎Z�o
		*pOut_velo_A = -weight_B * ConstVec + *pVelo_A;
		*pOut_velo_B = weight_A * ConstVec + *pVelo_B;

		// �Փˌ�ʒu�̎Z�o
		*pOut_pos_A = *pColliPos_A + time * (*pOut_velo_A);
		*pOut_pos_B = *pColliPos_B + time * (*pOut_velo_B);

		return true;
	}


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
	//	XMVECTOR *�p�[�e�B�N���̏Փˈʒu���i�[����XMVECTOR�^�ւ̃|�C���^
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


	// ���ƃ|���S���̏Փ˔���
	static bool SpherePolygonCollision(
		FLOAT r,
		XMVECTOR *pPre_pos, XMVECTOR *pPos,
		XMVECTOR *pNormal, XMVECTOR *pPlane_pos,
		FLOAT *t,
		XMVECTOR *pOut_colli
	)
	{
		XMVECTOR C0 = *pPre_pos - pPlane_pos[0]; // ���ʏ�̈�_���猻�݈ʒu�ւ̃x�N�g��
		XMVECTOR D = *pPos - *pPre_pos; // ���݈ʒu����\��ʒu�܂ł̃x�N�g��
		XMVECTOR N = XMVector3Normalize(*pNormal); // �@����W����


		// ���ʂƒ��S�_�̋������Z�o
		FLOAT Dot_C0;
		XMStoreFloat(&Dot_C0, XMVector3Dot(C0, N));
		//Dot_C0 -= r;

		FLOAT dist_plane_to_point = fabs(Dot_C0);

		// �i�s�����Ɩ@���̊֌W���`�F�b�N
		FLOAT Dot;
		XMStoreFloat(&Dot, XMVector3Dot(D, N));
		if (Dot >= 0)
			return false;

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
		*t = fabs(*t);

		// �Փˈʒu�̎Z�o
		*pOut_colli = *pPre_pos + (*t) * D;

		// �|���S�����̏Փˍ��W�擾
		D = XMVector3Normalize(D);
		XMVECTOR colPos = *pOut_colli + r * D;

		// �|���S�����ɏՓ˓_�����邩�ǂ���
		if (!PointInPolygon(&colPos, pPlane_pos))
			return false;

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


	// �|���S���̒��ɏՓ˓_�����邩�`�F�b�N
	static bool PointInPolygon(XMVECTOR *pInPolyPos, XMVECTOR *pPlane_pos)
	{
		XMVECTOR AB = pPlane_pos[1] - pPlane_pos[0];
		XMVECTOR BP = *pInPolyPos - pPlane_pos[1];

		XMVECTOR BC = pPlane_pos[2] - pPlane_pos[1];
		XMVECTOR CP = *pInPolyPos - pPlane_pos[2];
		
		XMVECTOR CA = pPlane_pos[0] - pPlane_pos[2];
		XMVECTOR AP = *pInPolyPos - pPlane_pos[0];

		XMVECTOR cross1 = XMVector3Cross(AB, BP);
		XMVECTOR cross2 = XMVector3Cross(BC, CP);
		XMVECTOR cross3 = XMVector3Cross(CA, AP);


		//���ςŏ��������t���������ׂ�
		float dot12;
		XMStoreFloat(&dot12, XMVector3Dot(cross1, cross2));
		float dot13;
		XMStoreFloat(&dot13, XMVector3Dot(cross1, cross3));


		if (dot12 >= 0 && dot13 >= 0) 
		{
			//�O�p�`�̓����ɓ_������
			return true;
		}
		else
			return false;
	}
};