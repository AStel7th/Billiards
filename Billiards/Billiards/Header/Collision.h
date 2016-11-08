#ifndef COLLISION_H
#define COLLISION_H

#include <Windows.h>
#include <DirectXMath.h>
using namespace DirectX;

#define EPSIRON 0.00001f	// 誤差

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
	//	XMVECTOR *パーティクルの衝突位置を格納するD3DXVECTOR型へのポインタ
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

	static bool SpherePolygonCollision(
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
		if ((EPSIRON - fabs(Dot) > 0.0f) && (dist_plane_to_point < r)) {
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
};

#endif 