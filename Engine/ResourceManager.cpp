#include "pch.h"
#include "ResourceManager.h"
#include "Model.h"

#include "Mesh.h"

ResourceManager* ResourceManager::m_pInstance = nullptr;

ResourceManager* ResourceManager::GetInstance()
{
	if(m_pInstance)
		return m_pInstance;
	else
	{
		m_pInstance = new ResourceManager;
		return m_pInstance;
	}
}

void ResourceManager::Setup(HWND hwnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDevcon)
{
	m_hwnd = hwnd;
	m_pDevice = pDevice;
	m_pDeviceContext = pDevcon;
}

std::shared_ptr<Texture> ResourceManager::CreateTexture(std::string type, std::wstring filepath)
{
	auto it = m_textureMap.find(filepath);
	if(it != m_textureMap.end())
	{
		std::shared_ptr<Texture> resourcePtr = it->second.lock();
		if(resourcePtr)
		{
			return resourcePtr;
		}
		else
		{
			m_textureMap.erase(it);
		}
	}

	std::shared_ptr<Texture> pTextureRV = std::make_shared<Texture>();
	pTextureRV->Create(type, filepath);
	m_textureMap[filepath] = pTextureRV;
	return pTextureRV;
}

std::shared_ptr<Model> ResourceManager::CreateModel(std::wstring filepath)
{
	auto it = m_modelMap.find(filepath);
	if (it != m_modelMap.end())
	{
		std::shared_ptr<Model> resourcePtr = it->second.lock();
		if (resourcePtr)
		{
			return resourcePtr;
		}
		else
		{
			m_modelMap.erase(it);
		}
	}

	std::shared_ptr<Model> pModelResource = std::make_unique<Model>(std::string(filepath.begin(), filepath.end()));
	pModelResource->Initialize(m_hwnd, m_pDevice, m_pDeviceContext);
	pModelResource->Load();

	m_modelMap[filepath] = pModelResource;
	return pModelResource;
}

