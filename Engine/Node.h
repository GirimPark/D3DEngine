#pragma once
#include <d3d11.h>
#include <string>
#include <vector>
#include <directxtk\SimpleMath.h>
using namespace DirectX::SimpleMath;

class Model;
class NodeAnimation;
class Mesh;

class Node
{
private:
	std::string m_name;
	Model* m_pOwner;
	Node* m_pParentNode;
	std::vector<Node*> m_pChildren;
	Matrix m_localTransform;
	Matrix m_worldTransform;
	NodeAnimation* m_pNodeAnimation;
	int m_curIdx = 0;
	float m_elapsedTime;
	float m_animationSpeed = 1.f;
	std::vector<Mesh*> m_meshes;

	ID3D11Device* m_pDevice;
	ID3D11Buffer* m_pNodeTransformConstantBuffer = nullptr;

public:
	void AddChildNode(Node* child);

public:
	Node(ID3D11Device* pDevice, std::string name, Model* pOwner, Matrix localTransform, const std::vector<Mesh*>& meshes);
	~Node();

public:
	void SetAnimation(NodeAnimation* pNodeAnimation) { this->m_pNodeAnimation = pNodeAnimation; }
	std::string GetName() { return m_name; }
	std::vector<Node*>& GetChildren() { return m_pChildren; }
	DirectX::XMMATRIX GetWorldTransform() { return m_localTransform; }
	void SetWorldTransform(DirectX::XMMATRIX transform) { m_localTransform = transform; }
	void SetAnimationSpeed(float speed) { m_animationSpeed = speed; }

public:
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* devcon);

private:
	void SetupNode();
	void UpdateTransform();
};

