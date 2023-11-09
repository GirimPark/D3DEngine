#include "pch.h"
#include "ModelLoader.h"

#include <directxtk/WICTextureLoader.h>
#include <fstream>
#include <string>

bool ModelLoader::Load(HWND hwnd, ID3D11Device* device, ID3D11DeviceContext* devcon, std::string fileName)
{
	Assimp::Importer importer;

	const aiScene* pScene = importer.ReadFile(fileName,
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_CalcTangentSpace |
		aiProcess_ConvertToLeftHanded);

	if (!pScene)
		return false;

	this->m_directory = fileName.substr(0, fileName.find_last_of("/\\"));
	this->m_device = device;
	this->m_deviceContext = devcon;
	this->m_HWND = hwnd;

	ProcessNode(pScene->mRootNode, pScene);

	return true;
}

void ModelLoader::Draw(ID3D11DeviceContext* devcon)
{
	for (auto& mesh : m_meshes)
	{
		mesh.Render(devcon);
	}
}

void ModelLoader::Close()
{
	for (auto& texture : m_loadedTextures)
	{
		texture.Release();
	}
}

void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (UINT i = 0; i<node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshes.push_back(this->ProcessMesh(mesh, scene));
	}

	for (UINT i = 0; i<node->mNumChildren; i++)
	{
		this->ProcessNode(node->mChildren[i], scene);
	}
}

Mesh ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> textures;

	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.X = mesh->mVertices[i].x;
		vertex.Y = mesh->mVertices[i].y;
		vertex.Z = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.UV.x = mesh->mTextureCoords[0][i].x;
			vertex.UV.y = mesh->mTextureCoords[0][i].y;
		}

		if(mesh->HasNormals())
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}

		if(mesh->HasTangentsAndBitangents())
		{
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i<mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];

		for(UINT j = 0; j<face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> normalMaps = this->LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", scene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> specularMaps = this->LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> emissiveMaps = this->LoadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive", scene);
		textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
		std::vector<Texture> opacityMaps = this->LoadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity", scene);
		textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());
	}

	return Mesh(m_device, vertices, indices, textures);
}

const std::vector<Texture>& ModelLoader::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName,
	const aiScene* scene)
{
	std::vector<Texture> textures;

	for (UINT i = 0; i < material->GetTextureCount(type); ++i)
	{
		aiString str;
		material->GetTexture(type, i, &str);

		bool skip = false;
		for (const auto& texture : m_loadedTextures)
		{
			if (std::strcmp(texture.Path.c_str(), str.C_Str()) == 0)
			{
				Texture temp = texture;
				temp.Type = typeName;
				textures.push_back(temp);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;

			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(str.C_Str());

			std::string fileName = std::string(str.C_Str());
			if(std::string::npos != fileName.find_last_of("/\\"))
			{
				fileName = "../Textures/" + fileName.substr(fileName.find_last_of("/\\"));
			}
			else
			{
				fileName = "../Textures/" + fileName;
			}
			fileName = m_directory + '/' + fileName;
			if(embeddedTexture)
			{
				bool result = SaveEmbeddedTexture(embeddedTexture, fileName);
				assert(result);
			}
			std::wstring fileNameWs = std::wstring(fileName.begin(), fileName.end());
			HR_T(CreateWICTextureFromFile(m_device, m_deviceContext, fileNameWs.c_str(), nullptr, &texture.Source));

			texture.Type = typeName;
			texture.Path = str.C_Str();
			textures.push_back(texture);
			this->m_loadedTextures.push_back(texture);
		}
	}

	return textures;
}

ID3D11ShaderResourceView* ModelLoader::LoadEmbeddedTexture(const aiTexture* embeddedTexture)
{
	ID3D11ShaderResourceView* texture = nullptr;

	if (embeddedTexture->mHeight)
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = embeddedTexture->mWidth;
		desc.Height = embeddedTexture->mHeight;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA subResourceData;
		subResourceData.pSysMem = embeddedTexture->pcData;
		subResourceData.SysMemPitch = embeddedTexture->mWidth * 4;
		subResourceData.SysMemSlicePitch = embeddedTexture->mWidth * embeddedTexture->mHeight * 4;

		ID3D11Texture2D* texture2D = nullptr;
		HR_T(m_device->CreateTexture2D(&desc, &subResourceData, &texture2D));
		HR_T(m_device->CreateShaderResourceView(texture2D, nullptr, &texture));

		return texture;
	}

	const size_t size = embeddedTexture->mWidth;
	HR_T(CreateWICTextureFromMemory(m_device, m_deviceContext, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture));
	return texture;
}

bool ModelLoader::SaveEmbeddedTexture(const aiTexture* embeddedTexture, std::string fileName)
{
	if(!embeddedTexture->mHeight)
	{
		// Save a compressed texture of mWidth bytes
		std::ofstream file(fileName.c_str(), std::ios::binary);
		file.write(reinterpret_cast<const char*>(embeddedTexture->pcData), embeddedTexture->mWidth);
		file.close();
	}
	else
	{
		// Save an uncompressed ARGB8888 embedded texture
		std::ofstream file(fileName.c_str(), std::ios::binary);
		file.write(reinterpret_cast<const char*>(embeddedTexture->pcData), embeddedTexture->mWidth * embeddedTexture->mHeight * 4);
		file.close();
	}

	return true;
}
