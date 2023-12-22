#include "pch.h"
#include "StaticMeshModel.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Mesh.h"

StaticMeshModel::StaticMeshModel(std::string name, HWND hwnd, const ComPtr<ID3D11Device>& pDevice, 
                                 const ComPtr<ID3D11DeviceContext>& pDevcon, std::string filename)
	: m_name(name)
	, m_HWND(hwnd)
	, m_pDevice(pDevice)
	, m_pDeviceContext(pDevcon)
	, m_fileName(filename)
	, m_directory(filename.substr(0, filename.find_last_of("/\\")))
{
	LoadSceneResource();
}

void StaticMeshModel::Update()
{
}

void StaticMeshModel::Render(ID3D11DeviceContext* devcon)
{
}

void StaticMeshModel::LoadSceneResource()
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

	ParsingNode(pScene->mRootNode, m_pRootNode.get(), pScene);
}

void StaticMeshModel::ParsingNode(aiNode* pNode, Node* pParentNode, const aiScene* pScene)
{
}

//Mesh* StaticMeshModel::ParsingMesh(aiMesh* mesh, const aiScene* pScene)
//{
//
//}
