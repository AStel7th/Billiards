#include "..\Header\CollisionFromFBX.h"

CollisionFromFBX::CollisionFromFBX()
{
	fbx = nullptr;
	m_nodeArray.clear();
}

CollisionFromFBX::~CollisionFromFBX()
{
	SAFE_DELETE(fbx);
}

HRESULT CollisionFromFBX::LoadFBX(const char * filename)
{
	if (!filename)
		return E_FAIL;

	HRESULT hr = S_OK;

	fbx = NEW FBX();
	hr = fbx->Initialaize(filename, eAXIS_DIRECTX);
	if (FAILED(hr))
		return hr;

	Setup();

	hr = CreateNodes();
	if (FAILED(hr))
		return hr;

	return hr;
}

void CollisionFromFBX::Setup()
{
	// RootNodeから探索していく
	if (fbx->mScene->GetRootNode())
	{
		SetupNode(fbx->mScene->GetRootNode());
	}
}

void CollisionFromFBX::SetupNode(FbxNode* pNode)
{
	if (!pNode)
		return;

	FBX_COLLISION_NODE colNode;

	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
	if (pAttrib)
	{
		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();
		if (type == FbxNodeAttribute::eMesh)
		{
			// メッシュにダウンキャスト
			FbxMesh *lMesh = (FbxMesh*)pAttrib;

			//事前に頂点数、ポリゴン数等を調べる
			colNode.vertexCount = lMesh->GetControlPointsCount();
			colNode.faceCount = lMesh->GetPolygonCount();

			if (colNode.vertexCount > 0)
			{
				// 頂点があるならノードにコピー
				CopyVertexData(lMesh, &colNode, pNode->LclTranslation, pNode->LclScaling, pNode->LclRotation);
			}
		}
	}

	//
	ComputeNodeMatrix(pNode, &colNode);

	m_fbxNodeArray.push_back(colNode);

	const int lCount = pNode->GetChildCount();
	for (int i = 0; i < lCount; i++)
	{
		SetupNode(pNode->GetChild(i));
	}
}

void CollisionFromFBX::CopyVertexData(FbxMesh*	pMesh, FBX_COLLISION_NODE* meshNode, FbxDouble3 translation, FbxDouble3 scale, FbxDouble3 rotation)
{
	if (!pMesh)
		return;

	meshNode->m_vertexDataArray.resize(meshNode->vertexCount);

	FbxVector4 pos, nor;

	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	int vertexId = 0;
	int index = 0; //index再定義用。

	for (int i = 0; i < meshNode->faceCount; i++)
	{
		int polygonSize = pMesh->GetPolygonSize(i);
		if (polygonSize != 3)
			return;

		for (int j = 0; j < polygonSize; j++)
		{
			FBX_VERTEX_DATA_COL vData;

			int lControlPointIndex = pMesh->GetPolygonVertex(i, j);

			int indexPos = pMesh->GetPolygonVertex(i, j);

			//index
			meshNode->m_indexArray.push_back(indexPos); index++;
			//pos       
			pos = lControlPoints[indexPos];

			vData.pos = pos;

			//normal
			for (int k = 0; k < pMesh->GetElementNormalCount(); k++) {
				FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal(k);
				//Mapping == eByPolygonVertex
				if (leNormals->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
					//Reference == eDirect
					if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect) {
						nor = leNormals->GetDirectArray().GetAt(lControlPointIndex);
						//Reference == eIndexToDirect
					}
					else if (leNormals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
						int id = leNormals->GetIndexArray().GetAt(lControlPointIndex);
						nor = leNormals->GetDirectArray().GetAt(id);
					}
					//Mapping == eByControlPoint
				}
				else if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
					//Reference == eDirect
					if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect) {
						nor = leNormals->GetDirectArray().GetAt(lControlPointIndex);
						//Reference == eIndexToDirect
					}
					else if (leNormals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
						int id = leNormals->GetIndexArray().GetAt(lControlPointIndex);
						nor = leNormals->GetDirectArray().GetAt(id);
					}
				}

				vData.nor = nor;
			}
			meshNode->m_vertexDataArray[lControlPointIndex] = vData;
		}
	}
}

void CollisionFromFBX::ComputeNodeMatrix(FbxNode* pNode, FBX_COLLISION_NODE* meshNode)
{
	XMMATRIX m(1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1);

	if (!pNode || !meshNode)
	{
		return;
	}

	FbxAnimEvaluator* lEvaluator = fbx->mScene->GetAnimationEvaluator();
	FbxMatrix lGlobal;
	lGlobal.SetIdentity();

	if (pNode != fbx->mScene->GetRootNode())
		lGlobal = lEvaluator->GetNodeGlobalTransform(pNode);

	for (int x = 0; x<4; x++)
	{
		for (int y = 0; y<4; y++)
		{
			meshNode->mLocal.m[y][x] = (float)lGlobal.Get(y, x);
		}
	}

	XMMATRIX mat;
	mat = XMLoadFloat4x4(&meshNode->mLocal);

	mat = XMMatrixMultiply(m, mat);
	XMStoreFloat4x4(&meshNode->mLocal, mat);
}

HRESULT CollisionFromFBX::CreateNodes()
{
	HRESULT hr = S_OK;

	size_t nodeCoount = m_fbxNodeArray.size();
	if (nodeCoount == 0)
		return E_FAIL;

	m_nodeArray.resize(nodeCoount);
	
	for (size_t i = 0; i<nodeCoount; i++)
	{
		NODE_COLLISION collisionNode;

		FBX_COLLISION_NODE fbxNode = m_fbxNodeArray[static_cast<unsigned int>(i)];
		
		collisionNode.m_polyDataArray.resize(fbxNode.faceCount);
		
		VertexConstruction(fbxNode, collisionNode);

		collisionNode.mLocal = fbxNode.mLocal;

		m_nodeArray[i] = collisionNode;
	}

	return hr;
}

HRESULT CollisionFromFBX::VertexConstruction(FBX_COLLISION_NODE & fbxNode, NODE_COLLISION & meshNode)
{
	DWORD vertexCount = static_cast<DWORD>(fbxNode.vertexCount);
	if (vertexCount == 0)
		return E_FAIL;

	HRESULT hr = S_OK;

	//	const size_t numUVSet = fbxNode.m_texcoordArray.size();

	for (int i = 0; i < fbxNode.faceCount; i++)
	{
		POLYGON_DATA polyData;
		for (int j = 0; j < 3; j++)
		{
			COLLISION_VERTEX_DATA vertexData;
			FBX_VERTEX_DATA_COL data = fbxNode.m_vertexDataArray[fbxNode.m_indexArray[i * 3 + j]];
			vertexData.vPos = DirectX::XMFLOAT3((float)data.pos.mData[0],
				(float)data.pos.mData[1],
				(float)data.pos.mData[2]);

			vertexData.vNor = DirectX::XMFLOAT3((float)data.nor.mData[0],
				(float)data.nor.mData[1],
				(float)data.nor.mData[2]);

			polyData.originVertexArray[j] = vertexData;
			polyData.vertexArray[j] = vertexData;
		}

		//ポリゴン法線算出
		XMVECTOR AB = XMLoadFloat3(&polyData.vertexArray[1].vPos) - XMLoadFloat3(&polyData.vertexArray[0].vPos);
		XMVECTOR BC = XMLoadFloat3(&polyData.vertexArray[2].vPos) - XMLoadFloat3(&polyData.vertexArray[1].vPos);

		XMVECTOR normal = XMVector3Cross(AB, BC);
		XMStoreFloat3(&polyData.normal,XMVector3Normalize(normal));

		meshNode.m_polyDataArray[i] = polyData;
	}

	return hr;
}

void CollisionFromFBX::Update()
{
	XMMATRIX _world = XMLoadFloat4x4(&world);
	for (auto it = m_nodeArray.begin(); it != m_nodeArray.end(); ++it)
	{
		XMMATRIX _local = XMLoadFloat4x4(&(*it).mLocal);
		XMMATRIX lw = _local * _world;
		for (auto it2 = (*it).m_polyDataArray.begin(); it2 != (*it).m_polyDataArray.end(); ++it2)
		{
			for (int i = 0; i < 3; ++i)
			{
				XMVECTOR _pos = XMLoadFloat3(&(*it2).originVertexArray[i].vPos);
				_pos = XMVector3Transform(_pos ,lw);

				XMStoreFloat3(&(*it2).vertexArray[i].vPos, _pos);
			}
		}
	}
}

void CollisionFromFBX::SetMatrix(XMFLOAT4X4 & w)
{
	world = w;
}

vector<NODE_COLLISION> CollisionFromFBX::GetMeshData()
{
	return m_nodeArray;
}
