#pragma once
#include <d3d11.h>
#include <map>
#include <memory>
#include <string>

struct Texture;
class Model;

class ResourceManager
{
private:
	/// 렌더러 분리하면 싱글톤인 CommonApp 통해서 사용, 임시로 넣어두고 씁니당
	HWND m_hwnd;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pDeviceContext = nullptr;

private:
	static ResourceManager* m_pInstance;

	std::map<std::wstring, std::weak_ptr<Texture>> m_textureMap;
	std::map<std::wstring, std::weak_ptr<Model>> m_modelMap;

public:
	static ResourceManager* GetInstance();

private:
	ResourceManager() = default;

public:
	void Setup(HWND hwnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDevcon);
	ID3D11Device* GetDevice() { return m_pDevice; }

	// Material, Texture 분리 후에는 타입 빼고 생성
	// 현재는 Material 클래스 생성 전이므로 타입까지 받는다
	std::shared_ptr<Texture> CreateTexture(std::string type, std::wstring filepath);
	std::shared_ptr<Model> CreateModel(std::wstring filepath);
};

