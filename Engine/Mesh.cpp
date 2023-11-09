#include "pch.h"
#include "Mesh.h"

Mesh::Mesh(ID3D11Device* device, const std::vector<Vertex>& vertices, const std::vector<UINT>& indices,
	const std::vector<Texture>& textures)
		: m_pDevice(device)
		, m_vertices(vertices)
		, m_indices(indices)
		, m_textures(textures)
		, m_pVertexBuffer(nullptr)
		, m_pIndexBuffer(nullptr)
{
	this->SetupMesh();
}

void Mesh::Render(ID3D11DeviceContext* devcon)
{
	TextureMapConstantBuffer TextureMapCB;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	devcon->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	devcon->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	devcon->PSSetConstantBuffers(2, 1, &m_pTextureMapConstantBuffer);

	for (const auto& texture : m_textures)
	{
		if (texture.Type == "texture_diffuse")
		{
			devcon->PSSetShaderResources(0, 1, &texture.Source);
			TextureMapCB.UseDiffuse = true;
		}
		if (texture.Type == "texture_normal")
		{
			devcon->PSSetShaderResources(1, 1, &texture.Source);
			TextureMapCB.UseNormal = true;
		}
		if (texture.Type == "texture_specular")
		{
			devcon->PSSetShaderResources(2, 1, &texture.Source);
			TextureMapCB.UseSpecular = true;
		}
		if (texture.Type == "texture_emissive")
		{
			devcon->PSSetShaderResources(3, 1, &texture.Source);
			TextureMapCB.UseEmissive = true;
		}
		if (texture.Type == "texture_opacity")
		{
			devcon->PSSetShaderResources(4, 1, &texture.Source);
			TextureMapCB.UseOpacity = true;
		}
	}
	devcon->UpdateSubresource(m_pTextureMapConstantBuffer, 0, nullptr, &TextureMapCB, 0, 0);

	devcon->DrawIndexed(static_cast<UINT>(m_indices.size()), 0, 0);
}

void Mesh::Finalize()
{
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);
	SAFE_RELEASE(m_pTextureMapConstantBuffer);

	m_vertices.clear();
	m_indices.clear();

	for(auto& texture:m_textures)
	{
		texture.Release();
	}
	m_textures.clear();
}

void Mesh::SetupMesh()
{
	D3D11_BUFFER_DESC vertexBD;
	vertexBD.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBD.ByteWidth = static_cast<UINT>(sizeof(Vertex) * m_vertices.size());
	vertexBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBD.CPUAccessFlags = 0;
	vertexBD.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = &m_vertices[0];
	HR_T(m_pDevice->CreateBuffer(&vertexBD, &initData, &m_pVertexBuffer));

	D3D11_BUFFER_DESC indexBD;
	indexBD.Usage = D3D11_USAGE_IMMUTABLE;
	indexBD.ByteWidth = static_cast<UINT>(sizeof(UINT) * m_indices.size());
	indexBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBD.CPUAccessFlags = 0;
	indexBD.MiscFlags = 0;

	initData.pSysMem = &m_indices[0];
	HR_T(m_pDevice->CreateBuffer(&indexBD, &initData, &m_pIndexBuffer));


	D3D11_BUFFER_DESC TCBDesc = {};
	ZeroMemory(&TCBDesc, sizeof(TCBDesc));
	TCBDesc.Usage = D3D11_USAGE_DEFAULT;
	TCBDesc.ByteWidth = sizeof(TextureMapConstantBuffer);
	TCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	TCBDesc.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&TCBDesc, nullptr, &m_pTextureMapConstantBuffer));
}