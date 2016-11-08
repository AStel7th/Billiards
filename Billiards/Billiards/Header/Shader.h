#pragma once

#include <d3d11.h>
#include <assert.h>

template <typename T> LPCSTR GetShaderProfileString();
template <typename T> T* CreateShader(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength);

// 頂点シェーダー
template <> inline LPCSTR GetShaderProfileString<ID3D11VertexShader>() { return "vs_5_0"; }
template <> inline 
ID3D11VertexShader* CreateShader<ID3D11VertexShader>(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength)
{
    ID3D11VertexShader *shader = 0;
    HRESULT hr = d3dDevice->CreateVertexShader(shaderBytecode, bytecodeLength, 0, &shader);
    if (FAILED(hr)) {
        // This shouldn't produce errors given proper bytecode, so a simple assert is fine
        assert(false);
    }
    return shader;
}

// Geometry shader
template <> inline LPCSTR GetShaderProfileString<ID3D11GeometryShader>() { return "gs_5_0"; }
template <> inline 
ID3D11GeometryShader* CreateShader<ID3D11GeometryShader>(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength)
{
    ID3D11GeometryShader *shader = 0;
    HRESULT hr = d3dDevice->CreateGeometryShader(shaderBytecode, bytecodeLength, 0, &shader);
    if (FAILED(hr)) {
        // This shouldn't produce errors given proper bytecode, so a simple assert is fine
        assert(false);
    }
    return shader;
}

// Pixel shader
template <> inline LPCSTR GetShaderProfileString<ID3D11PixelShader>() { return "ps_5_0"; }
template <> inline 
ID3D11PixelShader* CreateShader<ID3D11PixelShader>(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength)
{
    ID3D11PixelShader *shader = 0;
    HRESULT hr = d3dDevice->CreatePixelShader(shaderBytecode, bytecodeLength, 0, &shader);
    if (FAILED(hr)) {
        // This shouldn't produce errors given proper bytecode, so a simple assert is fine
        assert(false);
    }
    return shader;
}

// Compute shader
template <> inline LPCSTR GetShaderProfileString<ID3D11ComputeShader>() { return "cs_5_0"; }
template <> inline 
ID3D11ComputeShader* CreateShader<ID3D11ComputeShader>(ID3D11Device* d3dDevice, const void* shaderBytecode, size_t bytecodeLength)
{
    ID3D11ComputeShader *shader = 0;
    HRESULT hr = d3dDevice->CreateComputeShader(shaderBytecode, bytecodeLength, 0, &shader);
    if (FAILED(hr)) {
        // This shouldn't produce errors given proper bytecode, so a simple assert is fine
        assert(false);
    }
    return shader;
}

// Templated (on shader type) shader wrapper to wrap basic functionality
// TODO: Support optional lazy compile
template <typename T>
class Shader
{
public:
    Shader(ID3D11Device* d3dDevice, LPCTSTR srcFile, LPCSTR functionName, CONST D3D_SHADER_MACRO *defines = 0)
    {
        // TODO: Support profile selection from the application? Probably not necessary as we don't
        // support down-level hardware at the moment anyways.
        LPCSTR profile = GetShaderProfileString<T>();

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		// このフラグはシェーダのデバッグの経験を向上させるshaders.Settingにデバッグ情報を埋め込む
		// D3DCOMPILE_DEBUGフラグを設定しますが、それでもシェーダを最適化することが、彼らはこの
		// プログラムのリリース構成で実行される正確な方法を実行することができます。
		dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

        ID3DBlob *errors = 0;
        HRESULT hr = D3DCompileFromFile(srcFile, defines, 0, functionName, profile, 
			dwShaderFlags, 0, &bytecode, &errors);
        
        if (errors) {
            OutputDebugStringA(static_cast<const char *>(errors->GetBufferPointer()));
        }

        if (FAILED(hr)) {
            // TODO: Define exception type and improve this error string, but the ODS will do for now
            throw std::runtime_error("Error compiling shader");
        }

        mShader = CreateShader<T>(d3dDevice, bytecode->GetBufferPointer(), bytecode->GetBufferSize());
    }
    
    ~Shader() { mShader->Release(); }

    T* GetShader() { return mShader; }

	ID3DBlob* GetByteCode() { return bytecode.Get(); }

private:
    // Not implemented
    Shader(const Shader&);
    Shader& operator=(const Shader&);

    T* mShader;

	ComPtr<ID3DBlob> bytecode = 0;
};


typedef Shader<ID3D11VertexShader> VertexShader;
typedef Shader<ID3D11GeometryShader> GeometryShader;
typedef Shader<ID3D11PixelShader> PixelShader;
typedef Shader<ID3D11ComputeShader> ComputeShader;