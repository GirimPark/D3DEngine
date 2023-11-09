#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <directxtk\SimpleMath.h>
using namespace DirectX::SimpleMath;

class NodeAnimation;
class Mesh;

class Node
{
private:
	std::string m_name;
	Node* m_pParentNode;
	std::vector<Node*> m_pChildren;
	Matrix m_localTransform;
	Matrix m_worldTransform;
	NodeAnimation* m_pNodeAnimation;
	int m_curIdx = 0;
	std::vector<Mesh> m_meshes;

	ID3D11Device* m_pDevice;
	ID3D11Buffer* m_pNodeTransformConstantBuffer = nullptr;

	Matrix m_animatedTransform;

public:
	void AddChildNode(Node* child)
	{
		child->m_pParentNode = this;
		m_pChildren.push_back(child);
	}

public:
	Node(ID3D11Device* pDevice, std::string name, Matrix localTransform, const std::vector<Mesh>& meshes);

public:
	void SetAnimation(NodeAnimation* pNodeAnimation) { this->m_pNodeAnimation = pNodeAnimation; }
	std::string GetName() { return m_name; }
	std::vector<Node*>& GetChildren() { return m_pChildren; }

public:
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* devcon);
	void Finalize();

private:
	void SetupNode();
};

