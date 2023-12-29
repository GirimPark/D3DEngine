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
	/// ������ �и��ϸ� �̱����� CommonApp ���ؼ� ���, �ӽ÷� �־�ΰ� ���ϴ�
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

	// Material, Texture �и� �Ŀ��� Ÿ�� ���� ����
	// ����� Material Ŭ���� ���� ���̹Ƿ� Ÿ�Ա��� �޴´�
	std::shared_ptr<Texture> CreateTexture(std::string type, std::wstring filepath);
	std::shared_ptr<Model> CreateModel(std::wstring filepath);
};

