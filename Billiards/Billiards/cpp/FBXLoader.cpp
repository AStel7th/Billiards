#include "../Header/FBXLoader.h"
#include "../Header/MaterialData.h"
#include "../Header/Direct3D11.h"


FBXLoader::FBXLoader() : pFbx(nullptr)
{
	faceCount	= 0;
	vertexCount = 0;
	uvCount		= 0;
	indexCount	= 0;
	materialCount = 0;
	boneCount	= 0;
}

FBXLoader::~FBXLoader()
{
	Release();
}

//
void FBXLoader::Release()
{
	SAFE_DELETE(pFbx);
}

void FBXLoader::LoadFBX(const char* filename,string modelName)
{
	if (!filename)
		return;

	HRESULT hr = S_OK;

	pFbx = NEW FBX();
	hr = pFbx->Initialaize(filename, eAXIS_DIRECTX);	// sdk�}�l�[�W���[������
	if (FAILED(hr))
		return;

	meshData.SetName(modelName);		//���f���ɖ��O�ݒ�A���ʂɂ��g��

	Setup();
}

void FBXLoader::Setup()
{
	// RootNode����T�����Ă���
	if (pFbx->mScene->GetRootNode())
	{
		SetupNode(pFbx->mScene->GetRootNode(), "null");
	}
}

void FBXLoader::SetupNode(FbxNode* pNode, std::string parentName)
{
	if (!pNode)
		return;
	
	MESH mesh;

	mesh.name		= pNode->GetName();
	mesh.parentName = parentName;

	FbxNodeAttribute *pAttrib = pNode->GetNodeAttribute();
	if (pAttrib)
	{
		FbxNodeAttribute::EType type = pAttrib->GetAttributeType();
		if (type == FbxNodeAttribute::eMesh)
		{
			// ���b�V���Ƀ_�E���L���X�g
			FbxMesh *lMesh = (FbxMesh*)pAttrib;
			
			//���O�ɒ��_���A�|���S�������𒲂ׂ�
			vertexCount = lMesh->GetControlPointsCount();
			uvCount		= lMesh->GetTextureUVCount();
			faceCount	= lMesh->GetPolygonCount();

			//�{�[���̐�
			//�f�t�H�[�}�[�����݂��邩
			if (lMesh->GetDeformerCount(FbxDeformer::eSkin) == 0)
			{
				boneCount = 0;
			}
			else
			{
				FbxSkin* pSkinInfo = static_cast<FbxSkin*>(lMesh->GetDeformer(0, FbxDeformer::eSkin));
				boneCount = pSkinInfo->GetClusterCount();

				meshData.isAnimation = true;
			}

			if (vertexCount > 0)
			{
				vertexCount < uvCount ? vDataArray.resize(uvCount) : vDataArray.resize(vertexCount);
				
				// ���_������Ȃ�m�[�h�ɃR�s�[
				CopyVertexData(lMesh, &mesh, pNode->LclTranslation, pNode->LclScaling, pNode->LclRotation);

				if (boneCount > 0)
				{
					vertexCount < uvCount ? vbDataArray.resize(uvCount) : vbDataArray.resize(vertexCount);

					//���_����|���S�����t�����������̂ŁA�t�����e�[�u������� 
					vector<POLY_TABLE> polyTable;
					GetPolyTable(lMesh, &mesh, polyTable);

					// �{�[���R�s�[
					CopyBoneData(lMesh, &mesh, polyTable);
				}
			}


			// �}�e���A��
			materialCount = pNode->GetMaterialCount();
			mesh.materialData.resize(materialCount);
			for (int i = 0; i < materialCount; i++)
			{
				FbxSurfaceMaterial* mat = pNode->GetMaterial(i);
				if (!mat)
					continue;

				MaterialData* matData = MaterialManager::Instance().CreateMaterial(meshData.GetName() + "_" + mat->GetName());
				int indexCount;
				vector<int> pIndex;
				int faceNum;
				
				//���̃}�e���A���ł���C���f�b�N�X�z����̊J�n�C���f�b�N�X�𒲂ׂ�@����ɃC���f�b�N�X�̌������ׂ�		
				indexCount = 0;
				faceNum = faceCount;
				pIndex.resize(faceNum * 3);//�Ƃ肠�����A���b�V�����̃|���S�����Ń������m�ہi�X�̃|���S���O���[�v�͂��Ȃ炸����ȉ��ɂȂ邪�j

				for (int k = 0; k < faceNum; k++)
				{
					FbxLayerElementMaterial* mat = lMesh->GetLayer(0)->GetMaterials();//���C���[��1��������z��
					int matId = mat->GetIndexArray().GetAt(k);
					if (matId == i)
					{
						if (vertexCount < uvCount)
						{
							pIndex[indexCount]		= lMesh->GetTextureUVIndex(k, 0, FbxLayerElement::eTextureDiffuse);
							pIndex[indexCount + 1]	= lMesh->GetTextureUVIndex(k, 1, FbxLayerElement::eTextureDiffuse);
							pIndex[indexCount + 2]	= lMesh->GetTextureUVIndex(k, 2, FbxLayerElement::eTextureDiffuse);
						}
						else
						{

							pIndex[indexCount]		= lMesh->GetPolygonVertex(k, 0);
							pIndex[indexCount + 1]	= lMesh->GetPolygonVertex(k, 1);
							pIndex[indexCount + 2]	= lMesh->GetPolygonVertex(k, 2);
						}
						indexCount += 3;
					}
				}
				
				CopyMatrialData(mat,&mesh,indexCount,pIndex,*matData);

				mesh.materialData[i] = matData;
			}
		}
	}

	if (vertexCount != 0)
	{
		CreateVertexBuffer(&mesh.pVB, vDataArray.data(), sizeof(VERTEX_DATA), vDataArray.size());
		vDataArray.clear();

		if (boneCount > 0)
		{
			CreateVertexBuffer(&mesh.pVB_Bone, vbDataArray.data(), sizeof(BONE_DATA_PER_VERTEX), vbDataArray.size());
			vbDataArray.clear();
		}
	}
	
	ComputeNodeMatrix(pNode, &mesh);

	meshData.GetMeshList().push_back(mesh);

	const int lCount = pNode->GetChildCount();
	for (int i = 0; i < lCount; i++)
	{
		SetupNode(pNode->GetChild(i), mesh.name);
	}
}

//
void FBXLoader::SetFbxColor(XMFLOAT4& data, const FbxDouble3 srcColor)
{
	data.x = static_cast<float>(srcColor[0]);
	data.y = static_cast<float>(srcColor[1]);
	data.z = static_cast<float>(srcColor[2]);
	data.w = 1.0f;
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
void FBXLoader::CopyMatrialData(FbxSurfaceMaterial* mat, MESH* mesh, int indexCnt, vector<int>& indexArray, MaterialData& data)
{
	if (!mat)
		return;

	FBX_MATERIAL_NODE matNode;
	HRESULT hr;

	if (mat->GetClassId().Is(FbxSurfaceLambert::ClassId))
	{
		matNode.type = FBX_MATERIAL_NODE::MATERIAL_LAMBERT;
	}
	else if (mat->GetClassId().Is(FbxSurfacePhong::ClassId))
	{
		matNode.type = FBX_MATERIAL_NODE::MATERIAL_PHONG;
	}

	const FbxDouble3 lEmissive = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sEmissive, FbxSurfaceMaterial::sEmissiveFactor, &matNode.emmisive);
	SetFbxColor(data.emmisive, lEmissive);

	const FbxDouble3 lAmbient = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sAmbient, FbxSurfaceMaterial::sAmbientFactor, &matNode.ambient);
	SetFbxColor(data.ambient, lAmbient);

	const FbxDouble3 lDiffuse = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sDiffuse, FbxSurfaceMaterial::sDiffuseFactor, &matNode.diffuse);
	SetFbxColor(data.diffuse, lDiffuse);

	const FbxDouble3 lSpecular = GetMaterialProperty(mat,
		FbxSurfaceMaterial::sSpecular, FbxSurfaceMaterial::sSpecularFactor, &matNode.specular);
	SetFbxColor(data.specular, lSpecular);

	//
	FbxProperty lTransparencyFactorProperty = mat->FindProperty(FbxSurfaceMaterial::sTransparencyFactor);
	if (lTransparencyFactorProperty.IsValid())
	{
		double lTransparencyFactor = lTransparencyFactorProperty.Get<FbxDouble>();
		data.transparencyFactor = static_cast<float>(lTransparencyFactor);
	}

	// Specular Power
	FbxProperty lShininessProperty = mat->FindProperty(FbxSurfaceMaterial::sShininess);
	if (lShininessProperty.IsValid())
	{
		double lShininess = lShininessProperty.Get<FbxDouble>();
		data.specularPower = static_cast<float>(lShininess);
	}

	
	// Diffuse��������e�N�X�`����ǂݍ���
	if (matNode.diffuse.textureSetArray.size()>0)
	{
		TextureSet::const_iterator it = matNode.diffuse.textureSetArray.begin();
		if (it->second.size())
		{
			std::string path = it->second[0];
			data.SetTexture(path);
		}
	}

	ComPtr<ID3D11Buffer> indexBuffer;
	CreateIndexBuffer(indexCnt * sizeof(int), indexArray.data(), &indexBuffer);

	mesh->pIB.push_back(indexBuffer);

	data.faceCount = indexCnt / 3;//���̃}�e���A�����̃|���S����			
								  // samplerstate
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = Direct3D11::Instance().pD3DDevice->CreateSamplerState(&sampDesc, &data.pSampler);

	// material Constant Buffer
	D3D11_BUFFER_DESC bufDesc;
	ZeroMemory(&bufDesc, sizeof(bufDesc));
	bufDesc.ByteWidth = sizeof(MATERIAL_CONSTANT_DATA);
	bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufDesc.CPUAccessFlags = 0;

	hr = Direct3D11::Instance().pD3DDevice->CreateBuffer(&bufDesc, NULL, &data.pMaterialCb);

	data.materialConstantData.ambient = data.ambient;
	data.materialConstantData.diffuse = data.diffuse;
	data.materialConstantData.specular = data.specular;
	data.materialConstantData.emmisive = data.emmisive;
}


HRESULT FBXLoader::CreateVertexBuffer(ID3D11Buffer** pBuffer, void* pVertices, uint32_t stride, uint32_t vertexCount)
{
	if (stride == 0 || vertexCount == 0)
		return E_FAIL;

	HRESULT hr = S_OK;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = stride * vertexCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));

	InitData.pSysMem = pVertices;

	hr = Direct3D11::Instance().pD3DDevice->CreateBuffer(&bd, &InitData, pBuffer);
	if (FAILED(hr))
		return hr;

	return hr;
}

HRESULT FBXLoader::CreateIndexBuffer(DWORD dwSize, int* pIndex, ID3D11Buffer** ppIndexBuffer)
{
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = dwSize;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA InitData;
	InitData.pSysMem = pIndex;
	InitData.SysMemPitch = 0;
	InitData.SysMemSlicePitch = 0;
	if (FAILED(Direct3D11::Instance().pD3DDevice->CreateBuffer(&bd, &InitData, ppIndexBuffer)))
	{
		return FALSE;
	}

	return S_OK;
}


//
void FBXLoader::ComputeNodeMatrix(FbxNode* pNode, MESH* mesh)
{
	XMMATRIX m(1, 0, 0, 0,
		0, -1, 0, 0,
		0, 0, -1, 0,
		0, 0, 0, 1);

	if (!pNode || !mesh)
	{
		return;
	}

	FbxAnimEvaluator* lEvaluator = pFbx->mScene->GetAnimationEvaluator();
	FbxMatrix lGlobal;
	lGlobal.SetIdentity();

	if (pNode != pFbx->mScene->GetRootNode())
		lGlobal = lEvaluator->GetNodeGlobalTransform(pNode);

	for (int x = 0; x<4; x++)
	{
		for (int y = 0; y<4; y++)
		{
			mesh->mLocal.m[y][x] = (float)lGlobal.Get(y, x);
		}
	}
	XMMATRIX mat;
	mat = XMLoadFloat4x4(&mesh->mLocal);

	mat = XMMatrixMultiply(m, mat);
	XMStoreFloat4x4(&mesh->mLocal, mat);
}

//
void FBXLoader::CopyVertexData(FbxMesh*	pMesh, MESH* mesh,FbxDouble3 translation, FbxDouble3 scale, FbxDouble3 rotation)
{
	if (!pMesh)
		return;

	FbxVector4 pos, nor;

	FbxVector4* lControlPoints = pMesh->GetControlPoints();
	int vertexId = 0;

	int faceCount = pMesh->GetPolygonCount();

	for (int i = 0; i < faceCount; i++)
	{
		int polygonSize = pMesh->GetPolygonSize(i);
		if (polygonSize != 3)
			return;

		for (int j = 0; j < polygonSize; j++)
		{
			VERTEX_DATA vData;

			int lControlPointIndex =
				vertexCount < uvCount ? pMesh->GetTextureUVIndex(i, j, FbxLayerElement::eTextureDiffuse) : pMesh->GetPolygonVertex(i, j);

			int index = pMesh->GetPolygonVertex(i, j);

			//pos       
			pos = lControlPoints[index];
			vData.vPos = XMFLOAT3((float)pos.mData[0], (float)pos.mData[1], (float)pos.mData[2]);

			//normal
			vData.vNor = GetNormal(pMesh, lControlPointIndex);

			//uv
			vData.vTexcoord = GetUV(pMesh, lControlPointIndex, mesh, i, j);

			vDataArray[lControlPointIndex] = vData;
		}
	}
}

XMFLOAT3 FBXLoader::GetNormal(FbxMesh*	pMesh, int index)
{

	XMFLOAT3 vNor;
	FbxVector4 nor;
	for (int k = 0; k < pMesh->GetElementNormalCount(); k++)
	{
		FbxGeometryElementNormal* leNormals = pMesh->GetElementNormal(k);
		//Mapping == eByPolygonVertex
		if (leNormals->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			//Reference == eDirect
			if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect) {
				nor = leNormals->GetDirectArray().GetAt(index);
				//Reference == eIndexToDirect
			}
			else if (leNormals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				int id = leNormals->GetIndexArray().GetAt(index);
				nor = leNormals->GetDirectArray().GetAt(id);
			}
			//Mapping == eByControlPoint
		}
		else if (leNormals->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
			//Reference == eDirect
			if (leNormals->GetReferenceMode() == FbxGeometryElement::eDirect) {
				nor = leNormals->GetDirectArray().GetAt(index);
				//Reference == eIndexToDirect
			}
			else if (leNormals->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				int id = leNormals->GetIndexArray().GetAt(index);
				nor = leNormals->GetDirectArray().GetAt(id);
			}
		}

		vNor = XMFLOAT3((float)nor.mData[0], (float)nor.mData[1], (float)nor.mData[2]);
	}

	return vNor;
}

XMFLOAT2 FBXLoader::GetUV(FbxMesh* pMesh, int index,MESH* mesh,int polyNum,int inPolyPos)
{
	XMFLOAT2 vTexcoord;
	FbxVector2 uv;
	for (int k = 0; k < pMesh->GetElementUVCount(); k++) {
		FbxGeometryElementUV* leUV = pMesh->GetElementUV(k);
		//Mapping == eByPolygonVertex
		if (leUV->GetMappingMode() == FbxGeometryElement::eByPolygonVertex) {
			//Reference == eDirect
			//Reference == eIndexToDirect
			int lTextureUVIndex = pMesh->GetTextureUVIndex(polyNum, inPolyPos);
			uv = leUV->GetDirectArray().GetAt(lTextureUVIndex);
			//Mapping == eByControlPoint
		}
		else if (leUV->GetMappingMode() == FbxGeometryElement::eByControlPoint) {
			//Reference == eDirect
			if (leUV->GetReferenceMode() == FbxGeometryElement::eDirect) {
				uv = leUV->GetDirectArray().GetAt(index);
				//Reference == eIndexToDirect
			}
			else if (leUV->GetReferenceMode() == FbxGeometryElement::eIndexToDirect) {
				int id = leUV->GetIndexArray().GetAt(index);
				uv = leUV->GetDirectArray().GetAt(id);
			}
		}

		if ((float)uvCount > 0)
		{
			// �����UV1�������Ȃ�
			// UV��V�l���]
			vTexcoord = XMFLOAT2((float)abs(1.0f - uv.mData[0]), (float)abs(1.0f - uv.mData[1]));
			//vTexcoord = XMFLOAT2((float)uv.mData[0], (float)1.0f - uv.mData[1]);
		}
		else
			vTexcoord = XMFLOAT2(0, 0);
	}

	return vTexcoord;
}

vector<POLY_TABLE>& FBXLoader::GetPolyTable(FbxMesh * pMesh, MESH * mesh, vector<POLY_TABLE>& outArray)
{
	outArray.resize(vertexCount);

	for (int i = 0; i<vertexCount; i++)
	{
		for (int k = 0; k<faceCount; k++)
		{
			for (int m = 0; m<3; m++)
			{
				if (pMesh->GetPolygonVertex(k, m) == i)
				{
					outArray[i].PolyIndex[outArray[i].NumRef] = k;
					outArray[i].Index123[outArray[i].NumRef] = m;
					outArray[i].NumRef++;
				}
			}
		}
	}

	return outArray;
}

void FBXLoader::CopyBoneData(FbxMesh*	pMesh, MESH* mesh, vector<POLY_TABLE>& pPolyTable)
{
	//FBX���璊�o���ׂ����́A���_���Ƃ̃{�[���C���f�b�N�X�A���_���Ƃ̃{�[���E�F�C�g�A�o�C���h�s��A�|�[�Y�s��@��4����

	int i;

	//�f�t�H�[�}�[�𓾂� 
	if (pMesh->GetDeformerCount(FbxDeformer::eSkin) == 0)
		return;

	FbxSkin* pSkinInfo = static_cast<FbxSkin*>(pMesh->GetDeformer(0, FbxDeformer::eSkin));

	mesh->fbx_boneArray.resize(boneCount);
	for (i = 0; i<boneCount; i++)
	{
		mesh->fbx_boneArray[i] = pSkinInfo->GetCluster(i);
	}

	//FBX���璊�o���ׂ����́A���_���Ƃ̃{�[���C���f�b�N�X�A���_���Ƃ̃{�[���E�F�C�g�A�o�C���h�s��A�|�[�Y�s��@��4����

	int k;

	//�ʏ�̏ꍇ�@�i���_����=UV���@pvVB�����_�C���f�b�N�X�x�[�X�̏ꍇ�j
	if (vertexCount >= uvCount)
	{
		//���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�@��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���
		for (i = 0; i<boneCount; i++)
		{
			int iNumIndex = mesh->fbx_boneArray[i]->GetControlPointIndicesCount();//���̃{�[���ɉe�����󂯂钸�_��
			int* piIndex = mesh->fbx_boneArray[i]->GetControlPointIndices();
			double* pdWeight = mesh->fbx_boneArray[i]->GetControlPointWeights();
			//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
			for (k = 0; k<iNumIndex; k++)
			{
				for (int m = 0; m<4; m++)//FBX��CG�\�t�g���{�[��4�{�ȓ��Ƃ͌���Ȃ��B5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi�� 
				{
					if (pdWeight[k] > vbDataArray[piIndex[k]].boneWeight[m])
					{
						vbDataArray[piIndex[k]].boneIndex[m] = i;
						vbDataArray[piIndex[k]].boneWeight[m] = pdWeight[k];
						break;
					}
				}
			}
		}
	}
	//UV�C���f�b�N�X�x�[�X�̏ꍇ�@�i���_��<UV���j
	else
	{
		int PolyIndex = 0;
		int UVIndex = 0;
		//���ꂼ��̃{�[���ɉe�����󂯂钸�_�𒲂ׂ�@��������t�ɁA���_�x�[�X�Ń{�[���C���f�b�N�X�E�d�݂𐮓ڂ���
		for (i = 0; i<boneCount; i++)
		{
			int iNumIndex = mesh->fbx_boneArray[i]->GetControlPointIndicesCount();//���̃{�[���ɉe�����󂯂钸�_��
			int* piIndex = mesh->fbx_boneArray[i]->GetControlPointIndices();
			double* pdWeight = mesh->fbx_boneArray[i]->GetControlPointWeights();
			//���_������C���f�b�N�X�����ǂ��āA���_�T�C�h�Ő�������
			for (k = 0; k<iNumIndex; k++)
			{
				//���̒��_���܂�ł���|���S�����ׂĂɁA���̃{�[���ƃE�F�C�g��K�p
				for (int p = 0; p<pPolyTable[piIndex[k]].NumRef; p++)
				{
					//���_�������|���S�������̃|���S����UV�C���f�b�N�X�@�Ƌt����
					PolyIndex = pPolyTable[piIndex[k]].PolyIndex[p];
					UVIndex = pMesh->GetTextureUVIndex(PolyIndex, pPolyTable[piIndex[k]].Index123[p], FbxLayerElement::eTextureDiffuse);

					for (int m = 0; m<4; m++)//FBX��CG�\�t�g���{�[��4�{�ȓ��Ƃ͌���Ȃ��B5�{�ȏ�̏ꍇ�́A�d�݂̑傫������4�{�ɍi�� 
					{
						if (pdWeight[k] > vbDataArray[UVIndex].boneWeight[m])
						{
							vbDataArray[UVIndex].boneIndex[m] = i;
							vbDataArray[UVIndex].boneWeight[m] = pdWeight[k];
							break;
						}
					}
				}
			}
		}
	}

	XMMATRIX m(-1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1);

	//
	//�{�[���𐶐�
	mesh->boneArray.resize(boneCount);

	for (int i = 0; i<boneCount; i++)
	{
		FbxAMatrix mat;
		mesh->fbx_boneArray[i]->GetTransformLinkMatrix(mat);
		int a = mesh->fbx_boneArray[i]->GetControlPointIndicesCount();
		FbxMatrix initMat = FbxMatrix(mat);

		for (int x = 0; x<4; x++)
		{
			for (int y = 0; y<4; y++)
			{
				mesh->boneArray[i].mBindPose.m[y][x] = initMat.Get(y, x);
			}
		}

		/*XMMATRIX bindBone = XMLoadFloat4x4(&meshNode.boneArray[i].mBindPose);
		XMStoreFloat4x4(&meshNode.boneArray[i].mBindPose, XMMatrixMultiply(bindBone, m));*/
	}

	//SetNewPoseMatrix(fbxNode, &meshNode);
	//delete
}