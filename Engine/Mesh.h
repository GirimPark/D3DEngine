#pragma once

#include <string>
#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>
#include <directxtk/SimpleMath.h>

#include "Helper.h"

using namespace DirectX;

struct Vertex
{
	FLOAT X, Y, Z;
	XMFLOAT2 UV;
	XMFLOAT3 Normal;
	XMFLOAT3 Tangent;
};

struct Texture
{
	std::string Type;
	std::string Path;
	ID3D11ShaderResourceView* Source;

	void Release()
	{
		SAFE_RELEASE(Source);
	}
};

struct TextureMapConstantBuffer
{
	BOOL UseDiffuse = false;
	BOOL UseNormal = false;
	BOOL UseSpecular = false;
	BOOL UseEmissive = false;
	BOOL UseOpacity = false;

	SimpleMath::Vector3 garbage;
};

class Mesh
{
private:
	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;
	std::vector<Texture> m_textures;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;

	ID3D11Buffer* m_pTextureMapConstantBuffer = nullptr;

public:
	Mesh(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, const std::vector<Texture>& textures);

public:
	void Render(ID3D11DeviceContext* devcon);
	void Finalize();

private:
	void SetupMesh();
};

