#include "pch.h"
#include "Node.h"

#include "Animation.h"
#include "Mesh.h"

struct NodeTransformConstantBuffer
{
	Matrix mNodeWorld;
};

Node::Node(ID3D11Device* pDevice, std::string name, Matrix localTransform, const std::vector<Mesh*>& meshes)
	: m_pDevice(pDevice)
	, m_name(name)
	, m_localTransform(localTransform)
	, m_meshes(meshes)
{
	this->SetupNode();
}

Node::~Node()
{
	for (auto& node : m_pChildren)
	{
		SAFE_DELETE(node);
	}
	m_pChildren.clear();

	for(auto& mesh : m_meshes)
	{
		SAFE_DELETE(mesh);
	}
	m_meshes.clear();

	SAFE_RELEASE(m_pNodeTransformConstantBuffer);
}

void Node::Update(float deltaTime)
{
	if (m_pParentNode)
	{
		m_worldTransform = m_localTransform * m_pParentNode->m_worldTransform;
	}
	else
	{
		m_worldTransform = m_localTransform;
	}

	if (m_pNodeAnimation)
	{
		static float elapsedTime = 0.f;
		elapsedTime += deltaTime;

		if (elapsedTime > m_pNodeAnimation->Frames[m_curIdx].FrameDuration)
		{
			m_curIdx = (m_curIdx + 1) % (m_pNodeAnimation->Frames.size());
			elapsedTime = 0.f;

			m_animatedTransform = m_worldTransform * m_pNodeAnimation->Frames[m_curIdx].FrameTransform;
		}
	}

	for(const auto& child : m_pChildren)
	{
		child->Update(deltaTime);
	}
}

void Node::Render(ID3D11DeviceContext* devcon)
{
	devcon->VSSetConstantBuffers(3, 1, &m_pNodeTransformConstantBuffer);

	NodeTransformConstantBuffer NTCB;
	if(m_pNodeAnimation)
	{
		NTCB.mNodeWorld = XMMatrixTranspose(m_animatedTransform);
	}
	else
	{
		NTCB.mNodeWorld = XMMatrixTranspose(m_worldTransform);
	}
	devcon->UpdateSubresource(m_pNodeTransformConstantBuffer, 0, nullptr, &NTCB, 0, 0);

	for(auto& mesh : m_meshes)
	{
		mesh->Render(devcon);
	}

	for (const auto& child : m_pChildren)
	{
		child->Render(devcon);
	}
}

void Node::SetupNode()
{
	D3D11_BUFFER_DESC NTCBDesc = {};
	ZeroMemory(&NTCBDesc, sizeof(NTCBDesc));
	NTCBDesc.Usage = D3D11_USAGE_DEFAULT;
	NTCBDesc.ByteWidth = sizeof(NodeTransformConstantBuffer);
	NTCBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	NTCBDesc.CPUAccessFlags = 0;
	HR_T(m_pDevice->CreateBuffer(&NTCBDesc, nullptr, &m_pNodeTransformConstantBuffer));
}