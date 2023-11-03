#pragma once

#include <string>
#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>

using namespace DirectX;

#include "Helper.h"

struct Vertex
{
	FLOAT X, Y, Z;
	XMFLOAT2 texture;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
};

struct Texture
{
	std::string type;
	std::string path;
	ID3D11ShaderResourceView* texture;

	void Release()
	{
		SAFE_RELEASE(texture);
	}
};

class Mesh
{
public:
	std::vector<Vertex> m_vertices;
	std::vector<UINT> m_indices;
	std::vector<Texture> m_textures;
	INT m_textureMatrix;
	ID3D11Device* m_device;

	Mesh(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, const std::vector<Texture> textures)
		: m_device(device)
		, m_vertices(vertices)
		, m_indices(indices)
		, m_textures(textures)
		, m_vertexBuffer(nullptr)
		, m_indexBuffer(nullptr)
	{
		this->SetupMesh(this->m_device);
	}

	void Draw(ID3D11DeviceContext* devcon)
	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		devcon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
		devcon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		for(const auto& texture : m_textures)
		{
			if (texture.type == "texture_diffuse")
			{
				devcon->PSSetShaderResources(0, 1, &texture.texture);
				m_textureMatrix += 1;
			}
			else if (texture.type == "texture_normal")
			{
				devcon->PSSetShaderResources(1, 1, &texture.texture);
				m_textureMatrix += 2;
			}
			else if(texture.type == "texture_specular")
			{
				devcon->PSSetShaderResources(2, 1, &texture.texture);
				m_textureMatrix += 4;
			}
			else if (texture.type == "texture_emissive")
			{
				devcon->PSSetShaderResources(3, 1, &texture.texture);
				m_textureMatrix += 8;
			}
			else if (texture.type == "texture_opacity")
			{
				devcon->PSSetShaderResources(4, 1, &texture.texture);
				m_textureMatrix += 16;
			}
		}
		//devcon->UpdateSubresource()

		devcon->DrawIndexed(static_cast<UINT>(m_indices.size()), 0, 0);
	}

	void Close()
	{
		SAFE_RELEASE(m_vertexBuffer);
		SAFE_RELEASE(m_indexBuffer);
	}

private:
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	void SetupMesh(ID3D11Device* device)
	{
		D3D11_BUFFER_DESC vertexBD;
		vertexBD.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBD.ByteWidth = static_cast<UINT>(sizeof(Vertex) * m_vertices.size());
		vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBD.CPUAccessFlags = 0;
		vertexBD.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA initData;
		initData.pSysMem = &m_vertices[0];

		HR_T(device->CreateBuffer(&vertexBD, &initData, &m_vertexBuffer));

		D3D11_BUFFER_DESC indexBD;
		indexBD.Usage = D3D11_USAGE_IMMUTABLE;
		indexBD.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_indices.size());
		indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBD.CPUAccessFlags = 0;
		indexBD.MiscFlags = 0;

		initData.pSysMem = &m_indices[0];

		HR_T(device->CreateBuffer(&indexBD, &initData, &m_indexBuffer));
	}
};