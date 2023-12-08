#include "pch.h"
#include "Model.h"

#include <vector>
#include <DirectXMath.h>
#include <directxtk/WICTextureLoader.h>
#include <fstream>
#include <assimp/postprocess.h>

#include "Node.h"
#include "Mesh.h"
#include "Animation.h"

Model::Model(HWND hwnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDevcon, std::string fileName)
	: m_HWND(hwnd)
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDevcon)
	, m_fileName(fileName)
	, m_directory(fileName.substr(0, fileName.find_last_of("/\\")))
{
	
}

Model::~Model()
{
	for (auto& animation : m_pAnimations)
	{
		SAFE_DELETE(animation);
	}
	m_pAnimations.clear();

	SAFE_DELETE(m_pRootNode);

	SAFE_RELEASE(m_pBoneMatrixConstantBuffer);
}

void Model::Load()
{
	Assimp::Importer importer;
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const aiScene* pScene = importer.ReadFile(m_fileName,
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_GenUVCoords |
		aiProcess_CalcTangentSpace |
		aiProcess_LimitBoneWeights |
		aiProcess_ConvertToLeftHanded);

	assert(pScene);

	m_bones.resize(128);

	ParsingNode(pScene->mRootNode, m_pRootNode, pScene);
	m_bAnimation = ParsingAnimation(pScene);
	if(m_bAnimation)
	{
		AssignAnimation(m_pRootNode);
	}
	if(m_bBone)
	{
		AssignBone(m_pRootNode);
	}

	// Setup for Bone Matrix Update
	D3D11_BUFFER_DESC BMBDesc = {};
	ZeroMemory(&BMBDesc, sizeof(BMBDesc));
	BMBDesc.Usage = D3D11_USAGE_DEFAULT;
	BMBDesc.ByteWidth = sizeof(BoneMatrixConstantBuffer);
	BMBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	BMBDesc.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&BMBDesc, nullptr, &m_pBoneMatrixConstantBuffer));
}

void Model::Update(float deltaTime)
{
	m_pRootNode->Update(deltaTime);

	UpdateBoneMatrix();
}

void Model::Render(ID3D11DeviceContext* devcon)
{
	m_pRootNode->Render(devcon);
}

DirectX::XMMATRIX Model::GetTransform()
{
	return m_pRootNode->GetWorldTransform();
}

void Model::SetTransform(DirectX::XMMATRIX transform)
{
	m_pRootNode->SetWorldTransform(transform);
}


void Model::ParsingNode(aiNode* pNode, Node* pParentNode, const aiScene* pScene)
{
	std::vector<Mesh*> meshes;
	for(UINT i = 0; i<pNode->mNumMeshes; ++i)
	{
		aiMesh* pMesh = pScene->mMeshes[pNode->mMeshes[i]];
		meshes.push_back(this->ParsingMesh(pMesh, pScene));
	}
	
	Node* newNode = new Node{ m_pDevice, pNode->mName.C_Str(), this, ConvertaiMatrixToXMMatrix(pNode->mTransformation), meshes };
	if(pNode == pScene->mRootNode)
	{
		m_pRootNode = newNode;
	}
	else
	{
		pParentNode->AddChildNode(newNode);
	}

	// 재귀
	for(UINT i = 0; i<pNode->mNumChildren; ++i)
	{
		this->ParsingNode(pNode->mChildren[i], newNode, pScene);
	}
}

Mesh* Model::ParsingMesh(aiMesh* mesh, const aiScene* pScene)
{
	std::vector<Vertex> vertices;
	std::vector<UINT> indices;
	std::vector<Texture> textures;
	Vector4 baseColor;

	for(UINT i = 0; i<mesh->mNumVertices; i++)
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

		if (mesh->HasNormals())
		{
			vertex.Normal.x = mesh->mNormals[i].x;
			vertex.Normal.y = mesh->mNormals[i].y;
			vertex.Normal.z = mesh->mNormals[i].z;
		}

		if (mesh->HasTangentsAndBitangents())
		{
			vertex.Tangent.x = mesh->mTangents[i].x;
			vertex.Tangent.y = mesh->mTangents[i].y;
			vertex.Tangent.z = mesh->mTangents[i].z;
		}

		vertices.push_back(vertex);
	}

	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace& face = mesh->mFaces[i];

		for (UINT j = 0; j < face.mNumIndices; j++)
		{
			indices.push_back(face.mIndices[j]);
		}
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = pScene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = this->LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", pScene);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
		std::vector<Texture> normalMaps = this->LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal", pScene);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
		std::vector<Texture> specularMaps = this->LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", pScene);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
		std::vector<Texture> emissiveMaps = this->LoadMaterialTextures(material, aiTextureType_EMISSIVE, "texture_emissive", pScene);
		textures.insert(textures.end(), emissiveMaps.begin(), emissiveMaps.end());
		std::vector<Texture> opacityMaps = this->LoadMaterialTextures(material, aiTextureType_OPACITY, "texture_opacity", pScene);
		textures.insert(textures.end(), opacityMaps.begin(), opacityMaps.end());

		if(textures.size() == 0)
		{
			aiColor4D aiBaseColor;
			aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &aiBaseColor);
			
			baseColor.x = aiBaseColor.r;
			baseColor.y = aiBaseColor.g;
			baseColor.z = aiBaseColor.b;
			baseColor.w = aiBaseColor.a;
		}
	}

	if (mesh->HasBones())
	{
		m_bBone = true;
		ProcessBoneInfo(mesh, vertices);
	}
	else
	{
		m_bBone = false;
	}
	
	return new Mesh{ m_pDevice, vertices, indices, textures, baseColor };
}

void Model::ProcessBoneInfo(aiMesh* mesh, std::vector<Vertex>& vertices)
{
	// TODO : 본 노드 - 메시 노드 순서 말고 메시 노드 - 본 노드 순서로 저장된 fbx도 고려할 것
	// 1. 메시가 참조하는 본 정보 읽고 이름으로 중복 확인하기 - map 필요
	// 2. 중복이 아니라면 인덱스를 줘서 m_bones 벡터에 넣기
	//    bone 정보의 offsetMatrix 넣기
	//	  메시의 Vertex에는 참조하는 인덱스, 가중치 정보 넣기
	for(UINT i = 0; i<mesh->mNumBones; ++i)
	{
		aiBone* bone = mesh->mBones[i];
		std::string boneName = bone->mName.C_Str();
		if(m_boneMap.find(boneName) == m_boneMap.end())
		{
			m_boneIndex = m_indexCount++;

			m_boneMap[boneName] = m_boneIndex;

			m_bones[m_boneIndex].Name = boneName;
			m_bones[m_boneIndex].offsetMatrix = ConvertaiMatrixToXMMatrix(bone->mOffsetMatrix);
		}
		else
		{
			m_boneIndex = m_boneMap[boneName];
		}

		for (UINT j = 0; j < bone->mNumWeights; ++j)
		{
			vertices[bone->mWeights[j].mVertexId].AddBoneData(m_boneIndex, bone->mWeights[j].mWeight);
		}
	}
}

bool Model::ParsingAnimation(const aiScene* pScene)
{
	if(!pScene->HasAnimations())
		return false;

	for(UINT i=0; i<pScene->mNumAnimations; ++i)
	{
		Animation* animation = new Animation;
		animation->AnimationName = (pScene->mAnimations[i]->mName).C_Str();
		m_tickPerSecond = static_cast<float>(pScene->mAnimations[i]->mTicksPerSecond);
		animation->AnimationDuration = (pScene->mAnimations[i]->mDuration) / m_tickPerSecond;
		animation->NodeAnimations = ParsingNodeAnimation(pScene->mAnimations[i]);

		m_pAnimations.push_back(animation);
	}
	return true;
}

std::vector<NodeAnimation*> Model::ParsingNodeAnimation(aiAnimation* pAnimation)
{
	std::vector<NodeAnimation*> nodeAnimations;
	for(UINT i=0; i<pAnimation->mNumChannels; ++i)
	{
		NodeAnimation* animation = new NodeAnimation;
		animation->NodeName = (pAnimation->mChannels[i]->mNodeName).C_Str();
		animation->Frames = ParsingFrameKey(pAnimation->mChannels[i]);

		nodeAnimations.push_back(animation);
	}

	return nodeAnimations;
}

std::vector<FrameKey> Model::ParsingFrameKey(aiNodeAnim* pNodeAnim)
{
	std::vector<FrameKey> frames;
	for(UINT i=0; i<pNodeAnim->mNumPositionKeys; ++i)
	{
		FrameKey frame;

		frame.FramePosition.x = pNodeAnim->mPositionKeys[i].mValue.x;
		frame.FramePosition.y = pNodeAnim->mPositionKeys[i].mValue.y;
		frame.FramePosition.z = pNodeAnim->mPositionKeys[i].mValue.z;

		frame.FrameScale.x = pNodeAnim->mScalingKeys[i].mValue.x;
		frame.FrameScale.y = pNodeAnim->mScalingKeys[i].mValue.y;
		frame.FrameScale.z = pNodeAnim->mScalingKeys[i].mValue.z;

		frame.FrameQuaternion.x = pNodeAnim->mRotationKeys[i].mValue.x;
		frame.FrameQuaternion.y = pNodeAnim->mRotationKeys[i].mValue.y;
		frame.FrameQuaternion.z = pNodeAnim->mRotationKeys[i].mValue.z;
		frame.FrameQuaternion.w = pNodeAnim->mRotationKeys[i].mValue.w;
		
		frame.FrameTime = pNodeAnim->mPositionKeys[i].mTime / m_tickPerSecond;

		frames.push_back(frame);
	}

	return frames;
}

void Model::AssignAnimation(Node* node)
{
	for(const auto& animation : m_pAnimations[m_curAnimationIdx]->NodeAnimations)
	{
		if(node->GetName() == animation->NodeName)
		{
			node->SetAnimation(animation);
			break;
		}
	}

	for (const auto& child : node->GetChildren())
	{
		AssignAnimation(child);
	}
}

void Model::AssignBone(Node* node)
{
	// 노드를 순회하면서 m_bones 벡터의 Bone.pWorldMatrix가 해당 노드의 WorldMatrix를 참조하게 하기
	if(m_boneMap.find(node->GetName()) != m_boneMap.end() && !node->GetBoneLinked())
	{
		node->LinkWorldTransform(m_bones[m_boneMap[node->GetName()]].pBoneMatrix);
	}

	for(const auto& child : node->GetChildren())
	{
		AssignBone(child);
	}
}

void Model::UpdateBoneMatrix()
{
	assert(m_bones.size() <= 128);

	BoneMatrixConstantBuffer BoneMatrixCB;
	for(UINT i = 0; i< m_indexCount; ++i)
	{
		BoneMatrixCB.Array[i] = (m_bones[i].offsetMatrix * (*(m_bones[i].pBoneMatrix))).Transpose();
	}

	m_pDeviceContext->VSSetConstantBuffers(4, 1, &m_pBoneMatrixConstantBuffer);
	m_pDeviceContext->UpdateSubresource(m_pBoneMatrixConstantBuffer, 0, nullptr, &BoneMatrixCB, 0, 0);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName,
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
				temp.Source->AddRef();
				textures.push_back(temp);
				skip = true;
				break;
			}
		}
		if (!skip)
		{
			Texture texture;

			std::string fileName = std::string(str.C_Str());
			if(int at = fileName.find_last_of(".tga"))
			{
				fileName = fileName.substr(0, at-2) + "png";
			}

			const aiTexture* embeddedTexture = scene->GetEmbeddedTexture(fileName.c_str());

			if (std::string::npos != fileName.find_last_of("/\\"))
			{
				fileName = "../Textures/" + fileName.substr(fileName.find_last_of("/\\"));
			}
			else
			{
				fileName = "../Textures/" + fileName;
			}
			fileName = m_directory + '/' + fileName;
			if (embeddedTexture)
			{
				bool result = SaveEmbeddedTexture(embeddedTexture, fileName);
				assert(result);
			}
			std::wstring fileNameWs = std::wstring(fileName.begin(), fileName.end());
			HR_T(CreateWICTextureFromFile(m_pDevice, m_pDeviceContext, fileNameWs.c_str(), nullptr, &texture.Source));

			texture.Type = typeName;
			texture.Path = str.C_Str();
			textures.push_back(texture);
			this->m_loadedTextures.push_back(texture);
		}
	}

	return textures;
}

ID3D11ShaderResourceView* Model::LoadEmbeddedTexture(const aiTexture* embeddedTexture)
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
		HR_T(m_pDevice->CreateTexture2D(&desc, &subResourceData, &texture2D));
		HR_T(m_pDevice->CreateShaderResourceView(texture2D, nullptr, &texture));

		return texture;
	}

	const size_t size = embeddedTexture->mWidth;
	HR_T(CreateWICTextureFromMemory(m_pDevice, m_pDeviceContext, reinterpret_cast<const unsigned char*>(embeddedTexture->pcData), size, nullptr, &texture));
	return texture;
}

bool Model::SaveEmbeddedTexture(const aiTexture* embeddedTexture, std::string fileName)
{
	if (!embeddedTexture->mHeight)
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
