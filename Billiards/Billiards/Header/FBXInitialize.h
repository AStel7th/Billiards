#pragma once
#include "func.h"
#include <fbxsdk.h>
#include <Windows.h>

enum eAXIS_SYSTEM
{
	eAXIS_DIRECTX = 0,
	eAXIS_OPENGL,
};

static void FBXMatrixToFloat16(FbxMatrix* src, float dest[16])
{
	unsigned int nn = 0;
	for (int i = 0; i<4; i++)
	{
		for (int j = 0; j<4; j++)
		{
			dest[nn] = static_cast<float>(src->Get(i, j));
			nn++;
		}
	}
}

class FBX
{
public:

	// FBX SDK
	FbxManager* mSdkManager;
	FbxScene*	mScene;
	FbxImporter * mImporter;

	FBX()
	{
		mSdkManager = nullptr;
		mScene = nullptr;
		mImporter = nullptr;
	}

	~FBX()
	{
		if (mImporter)
		{
			mImporter->Destroy();
			mImporter = nullptr;
		}

		if (mScene)
		{
			mScene->Destroy();
			mScene = nullptr;
		}

		if (mSdkManager)
		{
			mSdkManager->Destroy();
			mSdkManager = nullptr;
		}
	}

	void TriangulateRecursive(FbxNode* pNode)
	{
		FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();

		if (lNodeAttribute)
		{
			if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
				lNodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
			{
				FbxGeometryConverter lConverter(pNode->GetFbxManager());
				// ����łǂ�Ȍ`����O�p�`��
#if 0
				lConverter.TriangulateInPlace(pNode);	// �Â���@
#endif // 0
				lConverter.Triangulate(mScene, true);
			}
		}

		const int lChildCount = pNode->GetChildCount();
		for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
		{
			// �q�m�[�h��T��
			TriangulateRecursive(pNode->GetChild(lChildIndex));
		}
	}

	HRESULT Initialaize(const char* filename, const eAXIS_SYSTEM axis = eAXIS_SYSTEM::eAXIS_OPENGL)
	{
		if (!filename)
			return E_FAIL;

		HRESULT hr = S_OK;

		//Fbx�}�l�[�W������
		mSdkManager = FbxManager::Create();
		if (!mSdkManager)
		{
			FBXSDK_printf("Error: Unable to create FBX Manager!\n");
			exit(1);
		}
		else FBXSDK_printf("Autodesk FBX SDK version %s\n", mSdkManager->GetVersion());

		//IOSettings�I�u�W�F�N�g���쐬���܂��B���̃I�u�W�F�N�g�͂��ׂẴC���|�[�g/�G�N�X�|�[�g�ݒ��ێ����Ă��܂��B
		FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
		mSdkManager->SetIOSettings(ios);

		//���s�f�B���N�g������ǂݍ��݃v���O�C���i�I�v�V�����j
		FbxString lPath = FbxGetApplicationDirectory();
		mSdkManager->LoadPluginsDirectory(lPath.Buffer());

		//FBX�V�[�����쐬���܂��B���̃I�u�W�F�N�g�́A�t�@�C���ւ�/����G�N�X�|�[�g�A�C���|�[�g�قƂ�ǂ̃I�u�W�F�N�g��/�ێ����Ă��܂��B
		mScene = FbxScene::Create(mSdkManager, "My Scene");
		if (!mScene)
		{
			FBXSDK_printf("Error: Unable to create FBX scene!\n");
			exit(1);
		}

		// �C���|�[�^�쐬
		mImporter = FbxImporter::Create(mSdkManager, "");

		int lFileFormat = -1;

		if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, lFileFormat))
		{
			// �F���ł��Ȃ��t�@�C���`�����o�C�i���`���ɕϊ��H
			lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}

		// �t�@�C������p���ăC���|�[�^�[������
		if (!mImporter || mImporter->Initialize(filename, lFileFormat, mSdkManager->GetIOSettings()) == false)
			return E_FAIL;

		//
		if (!mImporter || mImporter->Import(mScene) == false)
			return E_FAIL;

		FbxAxisSystem OurAxisSystem = FbxAxisSystem::DirectX;

		if (axis == eAXIS_OPENGL)
			OurAxisSystem = FbxAxisSystem::OpenGL;

		// DirectX�n
		FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
		if (SceneAxisSystem != OurAxisSystem)
		{
			FbxAxisSystem::DirectX.ConvertScene(mScene);
		}

		// �P�ʌn�̓���
		// �s�v�ł���������
		FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		{
			// �Z���`���[�^�[�P�ʂɃR���o�[�g����
			FbxSystemUnit::cm.ConvertScene(mScene);
		}

		// �O�p�`��(�O�p�`�ȊO�̃f�[�^�ł��R���ň��S)
		TriangulateRecursive(mScene->GetRootNode());

		return hr;
	}
};