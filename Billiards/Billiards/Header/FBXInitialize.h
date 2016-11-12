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
				// これでどんな形状も三角形化
#if 0
				lConverter.TriangulateInPlace(pNode);	// 古い手法
#endif // 0
				lConverter.Triangulate(mScene, true);
			}
		}

		const int lChildCount = pNode->GetChildCount();
		for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
		{
			// 子ノードを探索
			TriangulateRecursive(pNode->GetChild(lChildIndex));
		}
	}

	HRESULT Initialaize(const char* filename, const eAXIS_SYSTEM axis = eAXIS_SYSTEM::eAXIS_OPENGL)
	{
		if (!filename)
			return E_FAIL;

		HRESULT hr = S_OK;

		//Fbxマネージャ生成
		mSdkManager = FbxManager::Create();
		if (!mSdkManager)
		{
			FBXSDK_printf("Error: Unable to create FBX Manager!\n");
			exit(1);
		}
		else FBXSDK_printf("Autodesk FBX SDK version %s\n", mSdkManager->GetVersion());

		//IOSettingsオブジェクトを作成します。このオブジェクトはすべてのインポート/エクスポート設定を保持しています。
		FbxIOSettings* ios = FbxIOSettings::Create(mSdkManager, IOSROOT);
		mSdkManager->SetIOSettings(ios);

		//実行ディレクトリから読み込みプラグイン（オプション）
		FbxString lPath = FbxGetApplicationDirectory();
		mSdkManager->LoadPluginsDirectory(lPath.Buffer());

		//FBXシーンを作成します。このオブジェクトは、ファイルへの/からエクスポート、インポートほとんどのオブジェクトを/保持しています。
		mScene = FbxScene::Create(mSdkManager, "My Scene");
		if (!mScene)
		{
			FBXSDK_printf("Error: Unable to create FBX scene!\n");
			exit(1);
		}

		// インポータ作成
		mImporter = FbxImporter::Create(mSdkManager, "");

		int lFileFormat = -1;

		if (!mSdkManager->GetIOPluginRegistry()->DetectReaderFileFormat(filename, lFileFormat))
		{
			// 認識できないファイル形式をバイナリ形式に変換？
			lFileFormat = mSdkManager->GetIOPluginRegistry()->FindReaderIDByDescription("FBX binary (*.fbx)");;
		}

		// ファイル名を用いてインポーター初期化
		if (!mImporter || mImporter->Initialize(filename, lFileFormat, mSdkManager->GetIOSettings()) == false)
			return E_FAIL;

		//
		if (!mImporter || mImporter->Import(mScene) == false)
			return E_FAIL;

		FbxAxisSystem OurAxisSystem = FbxAxisSystem::DirectX;

		if (axis == eAXIS_OPENGL)
			OurAxisSystem = FbxAxisSystem::OpenGL;

		// DirectX系
		FbxAxisSystem SceneAxisSystem = mScene->GetGlobalSettings().GetAxisSystem();
		if (SceneAxisSystem != OurAxisSystem)
		{
			FbxAxisSystem::DirectX.ConvertScene(mScene);
		}

		// 単位系の統一
		// 不要でもいいかも
		FbxSystemUnit SceneSystemUnit = mScene->GetGlobalSettings().GetSystemUnit();
		if (SceneSystemUnit.GetScaleFactor() != 1.0)
		{
			// センチメーター単位にコンバートする
			FbxSystemUnit::cm.ConvertScene(mScene);
		}

		// 三角形化(三角形以外のデータでもコレで安心)
		TriangulateRecursive(mScene->GetRootNode());

		return hr;
	}
};