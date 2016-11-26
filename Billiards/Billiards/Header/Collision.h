#pragma once
#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

const float EPSIRON = 0.00001f;	// 誤差

class Collision
{

public:

	//AABB構造体
	typedef struct _AABB
	{
		XMFLOAT3 min;	//BOXの左下
		XMFLOAT3 max;
		XMFLOAT3 center;
		XMFLOAT3 size;
	}AABB;

	//球構造体
	typedef struct _SPHERE
	{
		XMFLOAT3 center;
		FLOAT radius;
	}SPHERE;


	///////////////////////////////////////////////////
	// パーティクル衝突判定・時刻・位置算出関数
	//   rA          : パーティクルAの半径
	//   rB          : パーティクルBの半径
	//   pre_pos_A   : パーティクルAの前の位置
	//   pos_A       : パーティクルAの次の到達位置
	//   pre_pos_B   : パーティクルBの前位置
	//   pos_B       : パーティクルBの次の到達位置
	//   pout_t      : 衝突時間を格納するFLOAT型へのポインタ
	//   pout_colli_A : パーティクルAの衝突位置を格納するD3DXVECTOR型へのポインタ
	//   pout_colli_B : パーティクルAの衝突位置を格納するD3DXVECTOR型へのポインタ

	static bool CheckParticleCollision(
		FLOAT rA, FLOAT rB,
		XMVECTOR *pPre_pos_A, XMVECTOR *pPos_A,
		XMVECTOR *pPre_pos_B, XMVECTOR *pPos_B,
		FLOAT *pOut_t,
		XMVECTOR *pOut_colli_A,
		XMVECTOR *pOut_colli_B
	)
	{
		// 前位置及び到達位置におけるパーティクル間のベクトルを算出
		XMVECTOR C0 = *pPre_pos_B - *pPre_pos_A;
		XMVECTOR C1 = *pPos_B - *pPos_A;
		XMVECTOR D = C1 - C0;

		// 衝突判定用の2次関数係数の算出
		FLOAT P;
		XMStoreFloat(&P,XMVector3LengthSq(D)); 
		if (P <= 0) 
			return false; // 同じ方向に移動

		FLOAT Q;
		XMStoreFloat(&Q,XMVector3Dot(C0, D)); 
		if (Q == 0) 
			return false; // 平行

		FLOAT R;
		XMStoreFloat(&R, XMVector3LengthSq(C0));

		// パーティクル距離
		FLOAT r = rA + rB;

		// 衝突判定式
		FLOAT Judge = Q*Q - P*(R - r*r);
		if (Judge < 0) {
			// 衝突していない
			return false;
		}

		// 衝突時間の算出
		FLOAT t_plus = (-Q + sqrt(Judge)) / P;
		FLOAT t_minus = (-Q - sqrt(Judge)) / P;

		// 衝突時間が0未満1より大きい場合、衝突しない
		//   if( (t_plus < 0 || t_plus > 1) && (t_minus < 0 || t_minus > 1)) return false;
		if (t_minus < 0 || t_minus > 1) return false;

		// 衝突時間の決定（t_minus側が常に最初の衝突）
		*pOut_t = t_minus;

		// 衝突位置の決定
		*pOut_colli_A = *pPre_pos_A + t_minus * (*pPos_A - *pPre_pos_A);
		*pOut_colli_B = *pPre_pos_B + t_minus * (*pPos_B - *pPre_pos_B);

		return true; // 衝突報告
	}


	///////////////////////////////////////////////////
	// パーティクル衝突後速度位置算出関数
	//   pColliPos_A : 衝突中のパーティクルAの中心位置
	//   pVelo_A     : 衝突の瞬間のパーティクルAの速度
	//   pColliPos_B : 衝突中のパーティクルBの中心位置
	//   pVelo_B     : 衝突の瞬間のパーティクルBの速度
	//   weight_A    : パーティクルAの質量
	//   weight_B    : パーティクルBの質量
	//   res_A       : パーティクルAの反発率
	//   res_B       : パーティクルBの反発率
	//   time        : 反射後の移動可能時間
	//   pOut_pos_A  : パーティクルAの反射後位置
	//   pOut_velo_A : パーティクルAの反射後速度ベクトル
	//   pOut_pos_B  : パーティクルBの反射後位置
	//   pOut_velo_B : パーティクルBの反射後速度ベクトル
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
		FLOAT TotalWeight = weight_A + weight_B; // 質量の合計
		FLOAT RefRate = (1 + res_A*res_B); // 反発率
		XMVECTOR C = *pColliPos_B - *pColliPos_A; // 衝突軸ベクトル
		C = XMVector3Normalize(C);
		FLOAT Dot;
		XMStoreFloat(&Dot,XMVector3Dot((*pVelo_A - *pVelo_B), C)); // 内積算出
		XMVECTOR ConstVec = RefRate*Dot / TotalWeight * C; // 定数ベクトル

															  // 衝突後速度ベクトルの算出
		*pOut_velo_A = -weight_B * ConstVec + *pVelo_A;
		*pOut_velo_B = weight_A * ConstVec + *pVelo_B;

		// 衝突後位置の算出
		*pOut_pos_A = *pColliPos_A + time * (*pOut_velo_A);
		*pOut_pos_B = *pColliPos_B + time * (*pOut_velo_B);

		return true;
	}


	//AABB同士のあたり判定
	static bool HitCheckAABB(const AABB& rcAABB1, const XMVECTOR& v,     // obj1の情報
		const AABB& rcAABB2, const XMVECTOR& v1)  // obj2の情報
	{ 
		// 相対速度を出す
		XMVECTOR rv = v - v1;

		//XMFLOAT3型に変換
		XMFLOAT3 rvf;
		XMStoreFloat3(&rvf, rv);

		// obj1を点として扱い、obj2を拡張する
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
				// 衝突点(ｙ方向)がAABBの線分に収まっていれば衝突
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
				// 衝突点(x方向)がAABBの線分に収まっていれば衝突
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
				// 衝突点(x方向)がAABBの線分に収まっていれば衝突
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

	//球とAABB
	static bool HitCheckSphereAndAABB(const SPHERE &sphere, const XMFLOAT3& v,     // 球の情報
		const AABB& rcAABB2, const XMVECTOR& v1)	//AABBの情報
	{
		FLOAT SqLen = 0;   // 長さのべき乗の値を格納
			
		// 各軸で点が最小値以下もしくは最大値以上ならば、差を考慮
		if (sphere.center.x + v.x < rcAABB2.min.x)  // i=0はX、1はY、2はZの意味です
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
	// 平面パーティクル衝突判定・時刻・位置算出関数
	// 戻り値 : 衝突(true), 非衝突(false)
	//static bool HitCheckSphereAndPlane(
	//	FLOAT パーティクルの半径,
	//	XMVECTOR *パーティクルの前の位置, XMVECTOR *パーティクルの次の到達位置,
	//	XMVECTOR *平面の法線, XMVECTOR *平面上の1点,
	//	FLOAT *衝突時間を格納するFLOAT型へのポインタ,
	//	XMVECTOR *パーティクルの衝突位置を格納するXMVECTOR型へのポインタ
	//)
	static bool HitCheckSphereAndPlane(
		FLOAT r,
		XMVECTOR *pPre_pos, XMVECTOR *pPos,
		XMVECTOR *pNormal, XMVECTOR *pPlane_pos,
		FLOAT *t,
		XMVECTOR *pOut_colli
		)
	{
		XMVECTOR C0 = *pPre_pos - *pPlane_pos; // 平面上の一点から現在位置へのベクトル
		XMVECTOR D = *pPos - *pPre_pos; // 現在位置から予定位置までのベクトル
		XMVECTOR N = XMVector3Normalize(*pNormal); // 法線を標準化


		// 平面と中心点の距離を算出
		FLOAT Dot_C0;
		XMStoreFloat(&Dot_C0, XMVector3Dot(C0, N));

		FLOAT dist_plane_to_point = fabs(Dot_C0);

		// 進行方向と法線の関係をチェック
		FLOAT Dot;
		XMStoreFloat(&Dot, XMVector3Dot(D, N));

		// 平面と平行に移動してめり込んでいるスペシャルケース
		if ((0.00001f - fabs(Dot) > 0.0f) && (dist_plane_to_point < r)){
			// 一生抜け出せないので最大時刻を返す
			*t = FLT_MAX;
			// 衝突位置は仕方ないので今の位置を指定
			*pOut_colli = *pPre_pos;
			return true;
		}

		// 交差時間の算出
		*t = (r - Dot_C0) / Dot;

		// 衝突位置の算出
		*pOut_colli = *pPre_pos + (*t) * D;

		// めり込んでいたら衝突として処理終了
		if (dist_plane_to_point < r)
			return true;

		// 壁に対して移動が逆向きなら衝突していない
		if (Dot >= 0)
			return false;

		// 時間が0～1の間にあれば衝突
		if ((0 <= *t) && (*t <= 1))
			return true;

		return false;
	}


	// 球とポリゴンの衝突判定
	static bool SpherePolygonCollision(
		FLOAT r,
		XMVECTOR *pPre_pos, XMVECTOR *pPos,
		XMVECTOR *pNormal, XMVECTOR *pPlane_pos,
		FLOAT *t,
		XMVECTOR *pOut_colli
	)
	{
		XMVECTOR C0 = *pPre_pos - pPlane_pos[0]; // 平面上の一点から現在位置へのベクトル
		XMVECTOR D = *pPos - *pPre_pos; // 現在位置から予定位置までのベクトル
		XMVECTOR N = XMVector3Normalize(*pNormal); // 法線を標準化


		// 平面と中心点の距離を算出
		FLOAT Dot_C0;
		XMStoreFloat(&Dot_C0, XMVector3Dot(C0, N));
		//Dot_C0 -= r;

		FLOAT dist_plane_to_point = fabs(Dot_C0);

		// 進行方向と法線の関係をチェック
		FLOAT Dot;
		XMStoreFloat(&Dot, XMVector3Dot(D, N));
		if (Dot >= 0)
			return false;

		// 平面と平行に移動してめり込んでいるスペシャルケース
		if ((EPSIRON - fabs(Dot) > 0.0f) && (dist_plane_to_point < r)) {
			// 一生抜け出せないので最大時刻を返す
			*t = FLT_MAX;
			// 衝突位置は仕方ないので今の位置を指定
			*pOut_colli = *pPre_pos;
			return true;
		}

		// 交差時間の算出
		*t = (r - Dot_C0) / Dot;
		*t = fabs(*t);

		// 衝突位置の算出
		*pOut_colli = *pPre_pos + (*t) * D;

		// ポリゴン内の衝突座標取得
		D = XMVector3Normalize(D);
		XMVECTOR colPos = *pOut_colli + r * D;

		// ポリゴン内に衝突点があるかどうか
		if (!PointInPolygon(&colPos, pPlane_pos))
			return false;

		// めり込んでいたら衝突として処理終了
		if (dist_plane_to_point < r)
			return true;

		// 壁に対して移動が逆向きなら衝突していない
		if (Dot >= 0)
			return false;

		// 時間が0～1の間にあれば衝突
		if ((0 <= *t) && (*t <= 1))
			return true;

		return false;
	}


	// ポリゴンの中に衝突点があるかチェック
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


		//内積で順方向か逆方向か調べる
		float dot12;
		XMStoreFloat(&dot12, XMVector3Dot(cross1, cross2));
		float dot13;
		XMStoreFloat(&dot13, XMVector3Dot(cross1, cross3));


		if (dot12 >= 0 && dot13 >= 0) 
		{
			//三角形の内側に点がある
			return true;
		}
		else
			return false;
	}
};