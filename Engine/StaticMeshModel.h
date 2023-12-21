#pragma once
#include <d3d11.h>
#include <memory>
#include <string>
#include <vector>

class Node;
class Mesh;



// �ִϸ��̼� ���� ����ƽ ��
class StaticMeshModel
{
private:
	std::string m_name;

	std::string m_directory;
	std::string m_filename;

	std::unique_ptr<Node> m_pRootNode;

	// ���� ��ȸ�� �������, ���� ������ ������ ��忡�� �Ѵ�.
	std::vector<std::weak_ptr<Mesh>> m_meshes;
	// Material�� �����ϴ� ���?

	// TODO: ���Ŀ� ���� �и��ϸ鼭 �����ϱ�
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

