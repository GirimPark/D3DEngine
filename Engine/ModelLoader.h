#pragma once

#include <vector>
#include <d3d11_1.h>
#include <DirectXMath.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"

using namespace DirectX;

class ModelLoader
{
public:
	ModelLoader() = default;
	~ModelLoader() = default;

	bool Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon, std::string fileName);
	void Draw(ID3D11DeviceContext* devcon);

	void Close();

private:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	std::vector<Mesh> m_meshes;
	std::string m_directory;
	std::vector<Texture> m_loadedTextures;
	HWND m_HWND = nullptr;



	void ProcessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* LoadEmbeddedTexture(const aiTexture* embeddedTexture);
};

