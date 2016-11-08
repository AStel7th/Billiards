#include "../Header/FBXLoader.h"

FBXLoader::FBXLoader()
{
	m_meshNodeArray.clear();
}

FBXLoader::~FBXLoader()
{
	Release();
}

//
void FBXLoader::Release()
{
	m_meshNodeArray.clear();

	//SAFE_DELETE(fbx);
}

HRESULT FBXLoader::LoadFBX(const char* filename)
{
	if (!filename)
		return E_FAIL;

	HRESULT hr = S_OK;

	fbx = make_unique<FBX>();
	hr = fbx->Initialaize(filename, eAXIS_DIRECTX);
	if (FAILED(hr))
		return hr;

	Setup();

	return hr;
}

//
FbxNode&	FBXLoader::GetRootNode()
{
	return *fbx->mScene->GetRootNode();
}

void FBXLoader::Setup()
{
	// RootNodeから探索していく
	if (fbx->mScene->GetRootNode())
	{
		SetupNode(fbx->mScene->GetRootNode(), "null");
	}
}

void FBXLoader::SetupNode(FbxNode* pNode, std::string parentName)
{
	if (!pNode)
		return;

	FBX_MESH_NODE meshNode;

	meshNode.name = pNode->GetName();
	meshNode.parentName = parentName;

	ZeroMemory(&meshNode.elements, sizeof(MESH_ELEMENTS));

	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
	if (pAttrib)
	{
		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();
		if (type == FbxNodeAttribute::eMesh)
		{
			// メッシュにダウンキャスト
			FbxMesh *lMesh = (FbxMesh*)pAttrib;
			meshNode.m_pMesh = lMesh;

			//事前に頂点数、ポリゴン数等を調べる
			meshNode.vertexCount = lMesh->GetControlPointsCount();
			meshNode.UVCount = lMesh->GetTextureUVCount();
			meshNode.faceCount = lMesh->GetPolygonCount();

			if (meshNode.vertexCount > 0)
			{
				// 頂点があるならノードにコピー
				CopyVertexData(lMesh, &meshNode, pNode->LclTranslation, pNode->LclScaling, pNode->LclRotation);

				// ボーンコピー
				CopyBoneData(lMesh, &meshNode);

				//頂点からポリゴンを逆引きしたいので、逆引きテーブルを作る 
				meshNode.polyTable.resize(meshNode.vertexCount);

				for (int i = 0; i<meshNode.vertexCount; i++)
				{
					for (int k = 0; k<meshNode.faceCount; k++)
					{
						for (int m = 0; m<3; m++)
						{
							if (lMesh->GetPolygonVertex(k, m) == i)
							{
								meshNode.polyTable[i].PolyIndex[meshNode.polyTable[i].NumRef] = k;
								meshNode.polyTable[i].Index123[meshNode.polyTable[i].NumRef] = m;
								meshNode.polyTable[i].NumRef++;
							}
						}
					}
				}
			}


			// マテリアル
			const int lMaterialCount = pNode->GetMaterialCount();
			for (int i = 0; i < lMaterialCount; i++)
			{
				FbxSurfaceMaterial* mat = pNode->GetMaterial(i);
				if (!mat)
					continue;

				FBX_MATERIAL_NODE destMat;
				CopyMatrialData(mat, &destMat);

				//そのマテリアルであるインデックス配列内の開始インデックスを調べる　さらにインデックスの個数も調べる		
				destMat.indexCount = 0;
				DWORD faceNum = meshNode.faceCount;
				destMat.pIndex.resize(faceNum * 3);//とりあえず、メッシュ内のポリゴン数でメモリ確保（個々のポリゴングループはかならずこれ以下になるが）

				for (int k = 0; k < faceNum; k++)
				{
					FbxLayerElementMaterial* mat = lMesh->GetLayer(0)->GetMaterials();//レイヤーが1枚だけを想定
					int matId = mat->GetIndexArray().GetAt(k);
					if (matId == i)
					{
						if (meshNode.vertexCount < meshNode.UVCount)
						{
							destMat.pIndex[destMat.indexCount] = lMesh->GetTextureUVIndex(k, 0, FbxLayerElement::eTextureDiffuse);
							destMat.pIndex[destMat.indexCount + 1] = lMesh->GetTextureUVIndex(k, 1, FbxLayerElement::eTextureDiffuse);
							destMat.pIndex[destMat.indexCount + 2] = lMesh->GetTextureUVIndex(k, 2, FbxLayerElement::eTextureDiffuse);
						}
						else
						{

							destMat.pIndex[destMat.indexCount] = lMesh->GetPolygonVertex(k, 0);
							destMat.pIndex[destMat.indexCount + 1] = lMesh->GetPolygonVertex(k, 1);
							destMat.pIndex[destMat.indexCount + 2] = lMesh->GetPolygonVertex(k, 2);
						}
						destMat.indexCount += 3;
					}
				}

				meshNode.m_materialArray.push_back(destMat);
			}
		}
	}

	//
	ComputeNodeMatrix(pNode, &meshNode);

	m_meshNodeArray.push_back(meshNode);

	const int lCount = pNode->GetChildCount();
	for (int i = 0; i < lCount; i++)
	{
		SetupNode(pNode->GetChild(i), meshNode.name);
	}
}

//
void FBXLoader::SetFbxColor(FBX_MATRIAL_ELEMENT& destColor, const FbxDouble3 srcColor)
{
	destColor.a = 1.0f;
	destColor.r = static_cast<float>(srcColor[0]);
	destColor.g = static_cast<float>(srcColor[1]);
	destColor.b = static_cast<float>(srcColor[2]);
}

//
FbxDouble3 FBXLoader::GetMaterialProperty(
	const FbxSurfaceMaterial * pMaterial,
	const char * pPropertyName,
	const char * pFactorPropertyName,
	FBX_MATRIAL_ELEMENT*			pElement)
{
	pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_NONE;

	FbxDouble3 lResult(0, 0, 0);
	const FbxProperty lProperty = pMaterial->FindProperty(pPropertyName);
	const FbxProperty lFactorProperty = pMaterial->FindProperty(pFactorPropertyName);
	if (lProperty.IsValid() && lFactorProperty.IsValid())
	{
		lResult = lProperty.Get<FbxDouble3>();
		double lFactor = lFactorProperty.Get<FbxDouble>();
		if (lFactor != 1)
		{
			lResult[0] *= lFactor;
			lResult[1] *= lFactor;
			lResult[2] *= lFactor;
		}

		pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_COLOR;
	}

	if (lProperty.IsValid())
	{
		int existTextureCount = 0;
		const int lTextureCount = lProperty.GetSrcObjectCount<FbxFileTexture>();

		for (int i = 0; i<lTextureCount; i++)
		{
			FbxFileTexture* lFileTexture = lProperty.GetSrcObject<FbxFileTexture>(i);
			if (!lFileTexture)
				continue;

			FbxString uvsetName = lFileTexture->UVSet.Get();
			std::string uvSetString = uvsetName.Buffer();
			std::string filepath = lFileTexture->GetRelativeFileName();

			pElement->textureSetArray[uvSetString].push_back(filepath);
			existTextureCount++;
		}

		const int lLayeredTextureCount = lProperty.GetSrcObjectCount<FbxLayeredTexture>();

		for (int i = 0; i<lLayeredTextureCount; i++)
		{
			FbxLayeredTexture* lLayeredTexture = lProperty.GetSrcObject<FbxLayeredTexture>(i);

			const int lTextureFileCount = lLayeredTexture->GetSrcObjectCount<FbxFileTexture>();

			for (int j = 0; j<lTextureFileCount; j++)
			{
				FbxFileTexture* lFileTexture = lLayeredTexture->GetSrcObject<FbxFileTexture>(j);
				if (!lFileTexture)
					continue;

				FbxString uvsetName = lFileTexture->UVSet.Get();
				std::string uvSetString = uvsetName.Buffer();
				std::string filepath = lFileTexture->GetFileName();

				pElement->textureSetArray[uvSetString].push_back(filepath);
				existTextureCount++;
			}
		}

		if (existTextureCount > 0)
		{
			if (pElement->type == FBX_MATRIAL_ELEMENT::ELEMENT_COLOR)
				pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_BOTH;
			else
				pElement->type = FBX_MATRIAL_ELEMENT::ELEMENT_TEXTURE;
		}
	}

	return lResult;
}

//
void FBXLoader::CopyMatrialData(FbxSurfaceMaterial* mat, FBX_MATERIAL_NODE* destMat)
{
	if (!mat)
		return;

	if (mat->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		destMat->type = FBX_MATERIAL_NODE::MATERIAL_LAMBERT;
	}
	else if (mat->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		destMat->type = FBX_MATERIAL_NODE::MATERIAL_PHONG;
	}

	const FbxDouble3 lEmissive = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &destMat->emmisive);
	SetFbxColor(destMat->emmisive, lEmissive);

	const FbxDouble3 lAmbient = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, &destMat->ambient);
	SetFbxColor(destMat->ambient, lAmbient);

	const FbxDouble3 lDiffuse = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, &destMat->diffuse);
	SetFbxColor(destMat->diffuse, lDiffuse);

	const FbxDouble3 lSpecular = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &destMat->specular);
	SetFbxColor(destMat->specular, lSpecular);

	//
	FbxProperty lTransparencyFactorProperty = mat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (lTransparencyFactorProperty.IsValid())
	{
		double lTransparencyFactor = lTransparencyFactorProperty.Get<FbxDouble>();
		destMat->TransparencyFactor = static_cast<float>(lTransparencyFactor);
	}

	// Specular Power
	FbxProperty lShininessProperty = mat->FindProperty(FbxSurfaceMaterial::sShininess);
	if (lShininessProperty.IsValid())
	{
		double lShininess = lShininessProperty.Get<FbxDouble>();
		destMat->shininess = static_cast<float>(lShininess);
	}
}

//
void FBXLoader::ComputeNodeMatrix(FbxNode* pNode, FBX_MESH_NODE* meshNode)
{
	if (!pNode || !meshNode)
	{
		return;
	}

	FbxAnimEvaluator* lEvaluator = fbx->mScene->GetAnimationEvaluator();
	FbxMatrix lGlobal;
	lGlobal.SetIdentity();

	if (pNode != fbx->mScene->GetRootNode())
	{
		lGlobal = lEvaluator->GetNodeGlobalTransform(pNode);

		FBXMatrixToFloat16(&lGlobal, meshNode->mat4x4);
	}
	else
	{
		FBXMatrixToFloat16(&lGlobal, meshNode->mat4x4);
	}
}

//
void FBXLoader::CopyVertexData(FbxMesh*	pMesh, FBX_MESH_NODE* meshNode, FbxDouble3 translation, FbxDouble3 scale, FbxDouble3 rotation)
{
	if (!pMesh)
		return;

	meshNode->vertexCount < meshNode->UVCount ? meshNode->m_vertexDataArray.resize(meshNode->UVCount) : meshNode->m_vertexDataArray.resize(meshNode->vertexCount);
	//meshNode->m_vertexDataArray.reserve(meshNode->faceCount * 3);

	FbxVector4 pos, nor;

	meshNode->elements.numPosition = 1;
	meshNode->elements.numNormal = 1;

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
			FBX_VERTEX_DATA vData;

			int lControlPointIndex =
				meshNode->vertexCount < meshNode->UVCount ? pMesh->GetTextureUVIndex(i, j, FbxLayerElement::eTextureDiffuse) : pMesh->GetPolygonVertex(i, j);

			int indexPos = pMesh->GetPolygonVertex(i, j);

			//index
			meshNode->m_indexArray.push_back(index); index++;
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

			//uv
			FbxVector2 uv;
			for (int k = 0; k < pMesh->GetElementUVCount(); k++) {
				FbxGeometryElementUV* leUV = pMesh->GetElementUV(k);
				//Mapping == eByPolygonVertex
				if (leUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
					//Reference == eDirect
					//Reference == eIndexToDirect
					int lTextureUVIndex = pMesh->GetTextureUVIndex(i, j);
					uv = leUV->GetDirectArray().GetAt(lTextureUVIndex);
					//Mapping == eByControlPoint
				}
				else if (leUV->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
					//Reference == eDirect
					if (leUV->GetReferenceMode() == FbxGeometryElement::eDirect) {
						uv = leUV->GetDirectArray().GetAt(lControlPointIndex);
						//Reference == eIndexToDirect
					}
					else if (leUV->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
						int id = leUV->GetIndexArray().GetAt(lControlPointIndex);
						uv = leUV->GetDirectArray().GetAt(id);
					}
				}
				vData.uv = uv;
			}
			meshNode->m_vertexDataArray[lControlPointIndex] = vData;
		}
	}
}

void FBXLoader::CopyBoneData(FbxMesh*	pMesh, FBX_MESH_NODE* meshNode)
{
	//FBXから抽出すべき情報は、頂点ごとのボーンインデックス、頂点ごとのボーンウェイト、バインド行列、ポーズ行列　の4項目

	int i;
	int iNumBone = 0;//ボーン数

					 //デフォーマーを得る 
	if (pMesh->GetDeformerCount(FbxDeformer::eSkin) == 0)
		return;
	FbxSkin* pSkinInfo = static_cast<FbxSkin*>(pMesh->GetDeformer(0, FbxDeformer::eSkin));
	//
	//ボーンを得る
	iNumBone = pSkinInfo->GetClusterCount();
	if (iNumBone == 0)
	{
		meshNode->anim = false;
	}
	meshNode->m_boneArray.resize(iNumBone);
	for (i = 0; i<iNumBone; i++)
	{
		meshNode->m_boneArray[i] = pSkinInfo->GetCluster(i);
	}
}

FBX_MESH_NODE& FBXLoader::GetNode(const unsigned int id)
{
	return m_meshNodeArray[id];
}
//
////ボーンを次のポーズ位置にセットする
//void FBXLoader::SetNewPoseMatrices(int frame)
//{
//	int i,j;
//	FbxTime time;
//	time.SetTime(0, 0, 0, frame, 0, FbxTime::eFrames60);//30フレーム/秒　と推定　厳密には状況ごとに調べる必要あり
//
//	for (i = 0; i < m_meshNodeArray.size(); i++)
//	{
//		for (j = 0; j < m_meshNodeArray[i].m_boneArray.size(); j++)
//		{
//			//BONEの新しい状態取得
//			m_meshNodeArray[i].m_boneArray[j]->SetTransformLinkMatrix(m_meshNodeArray[i].m_boneArray[j]->GetLink()->EvaluateGlobalTransform(time));
//
//			//for (int x = 0; x<4; x++)
//			//{
//			//	for (int y = 0; y<4; y++)
//			//	{
//			//		//行列代入
//			//		node->m_boneArray[i]->SetTransformLinkMatrix()
//			//		node->boneArray[i].mNewPose.r[y].m128_f32[x] = mat.Get(y, x);
//			//	}
//			//}
//		}
//	}
//	
//}