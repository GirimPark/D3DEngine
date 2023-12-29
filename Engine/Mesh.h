#pragma once

#include <string>
#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <__msvc_filebuf.hpp>
#include <directxtk/SimpleMath.h>
#include <directxtk/WICTextureLoader.h>
#include <wrl/client.h>

#include "Helper.h"
#include "ResourceManager.h"

using namespace DirectX;

struct Vertex
{
	FLOAT X, Y, Z;
	XMFLOAT2 UV;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
	INT BlendIndices[4] = {};	// �����ϴ� ���� �ε���
	float BlendWeights[4] = {0.f, 0.f, 0.f, 0.f};	// �����ϴ� ���� ���� ����ġ

	void AddBoneData(int boneIndex, float weight)
	{
		// �����Ͱ� �ϳ��� ����־�� �Ѵ�
		assert(BlendWeights[0] == 0.f || BlendWeights[1] == 0.f || BlendWeights[2] == 0.f || BlendWeights[3] == 0.f);
		for(UINT i = 0; i<4; ++i)
		{
			if(BlendWeights[i]==0.f)
			{
				BlendIndices[i] = boneIndex;
				BlendWeights[i] = weight;
				return;
			}
		}
	}
};

struct Texture
{
	std::string Type;
	std::wstring Path;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Source;

	void Create(std::string type, std::wstring filepath)
	{
		Type = type;
		Path = filepath;
		CreateWICTextureFromFile(ResourceManager::GetInstance()->GetDevice(), Path.c_str(), nullptr, Source.GetAddressOf());
	}
};

struct TextureMapConstantBuffer
{
	BOOL UseDiffuse = false;
	BOOL UseNormal = false;
	BOOL UseSpecular = false;
	BOOL UseEmissive = false;
	BOOL UseOpacity = false;
	BOOL UseMetalness = false;
	BOOL UseRoughness = false;

	BOOL garbage;

	SimpleMath::Vector4 BaseColor = {1.f, 1.f, 1.f, 1.f};
};

class Mesh
{
private:
	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;
	std::vector<std::shared_ptr<Texture>> m_textures;
	SimpleMath::Vector4 m_baseColor;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	ID3D11Buffer* m_pTextureMapConstantBuffer = nullptr;

public:
	Mesh(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, std::vector<std::shared_ptr<Texture>> textures, SimpleMath::Vector4 baseColor);
	~Mesh();

public:
	void Render(ID3D11DeviceContext* devcon);

private:
	void SetupMesh();
};