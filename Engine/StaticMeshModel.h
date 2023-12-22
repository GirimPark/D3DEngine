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

// 애니메이션 없는 스태틱 모델
class StaticMeshModel
{
private:
	std::string m_name;

	std::string m_directory;
	std::string m_fileName;

	std::unique_ptr<Node> m_pRootNode;

	// 정보 조회용 멤버변수, 실제 데이터 조작은 노드에서 한다.
	std::vector<std::weak_ptr<Mesh>> m_meshes;

	// TODO: 이후에 구조 분리하면서 삭제하기
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

