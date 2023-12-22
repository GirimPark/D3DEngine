#pragma once

#include "Node.h"

#include <d3d11.h>
#include <memory>
#include <string>
#include <vector>
#include <wrl.h>

struct aiNode;
struct aiScene;
struct aiMesh;
class Mesh;

using namespace Microsoft::WRL;

// �ִϸ��̼� ���� ����ƽ ��
class StaticMeshModel
{
private:
	std::string m_name;

	std::string m_directory;
	std::string m_fileName;

	std::unique_ptr<Node> m_pRootNode;

	// ���� ��ȸ�� �������, ���� ������ ������ ��忡�� �Ѵ�.
	std::vector<std::weak_ptr<Mesh>> m_meshes;

	// TODO: ���Ŀ� ���� �и��ϸ鼭 �����ϱ�
	HWND m_HWND;
	ComPtr<ID3D11Device> m_pDevice;
	ComPtr<ID3D11DeviceContext> m_pDeviceContext;

public:
	StaticMeshModel(std::string name, HWND hwnd, 
		const ComPtr<ID3D11Device>& pDevice, const ComPtr<ID3D11DeviceContext>& pDevcon, std::string filename);
	~StaticMeshModel() = default;

public:
	void Update();
	void Render(ID3D11DeviceContext* devcon);

private:
	void LoadSceneResource();

	void ParsingNode(aiNode* pNode, Node* pParentNode, const aiScene* pScene);
	//Mesh* ParsingMesh(aiMesh* mesh, const aiScene* pScene);


};

