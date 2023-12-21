#pragma once
#include <d3d11.h>
#include <memory>
#include <string>
#include <vector>

class Node;
class Mesh;



// 애니메이션 없는 스태틱 모델
class StaticMeshModel
{
private:
	std::string m_name;

	std::string m_directory;
	std::string m_filename;

	std::unique_ptr<Node> m_pRootNode;

	// 정보 조회용 멤버변수, 실제 데이터 조작은 노드에서 한다.
	std::vector<std::weak_ptr<Mesh>> m_meshes;
	// Material을 재사용하는 경우?

	// TODO: 이후에 구조 분리하면서 삭제하기
	 ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pDeviceContext;

public:
	StaticMeshModel(std::string name);
	~StaticMeshModel() = default;

public:
	void Update();
	void Render(ID3D11DeviceContext* devcon);

public:
	void AssignSceneResource(std::string filename);
	void AssignAnimation(std::string filename);
};

