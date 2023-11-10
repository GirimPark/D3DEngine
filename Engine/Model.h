#pragma once

#include <d3d11_1.h>
#include <DirectXMath.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <vector>

#include "Animation.h"


struct FrameKey;
struct NodeAnimation;
struct Texture;
class Node;
class Animation;
class Mesh;

class Model
{
private:
	Node* m_pRootNode;
	std::vector<Animation*> m_pAnimations;
	int m_curAnimationIdx = 0;
	bool m_bAnimation;
	float m_curAnimationDuration;	// 마지막 프레임 지속시간 계산을 위한 임시 변수
	float m_tickPerSecond;

	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;
	HWND m_HWND;
	std::string m_directory;
	std::string m_fileName;

	std::vector<Texture> m_loadedTextures;

	Assimp::Importer m_Importer;

public:
	Model(HWND hwnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDevcon, std::string fileName);
	~Model();

	void Load();
	void Update(float deltaTime);
	void Render(ID3D11DeviceContext* devcon);

public:
	DirectX::XMMATRIX GetTransform();
	void SetTransform(DirectX::XMMATRIX transform);
	void SetAnimationSpeed(float speed) { m_pAnimations[m_curAnimationIdx]->AnimationSpeed = speed; }
	float GetAnimationSpeed() { return m_pAnimations[m_curAnimationIdx]->AnimationSpeed; }
	float GetAnimationDuration() { return m_pAnimations[m_curAnimationDuration]->AnimationDuration; }

private:
	void ParsingNode(aiNode* pNode, Node* pParentNode, const aiScene* pScene);
	Mesh* ParsingMesh(aiMesh* mesh, const aiScene* pScene);
	bool ParsingAnimation(const aiScene* pScene);
	std::vector<NodeAnimation*> ParsingNodeAnimation(aiAnimation* pAnimation);
	std::vector<FrameKey> ParsingFrameKey(aiNodeAnim* pNodeAnim);
	void AssignAnimation(Node* node);

	std::vector<Texture> LoadMaterialTextures(aiMaterial* material, aiTextureType type, std::string typeName, const aiScene* scene);
	ID3D11ShaderResourceView* LoadEmbeddedTexture(const aiTexture* embeddedTexture);
	bool SaveEmbeddedTexture(const aiTexture* embeddedTexture, std::string fileName);
};

