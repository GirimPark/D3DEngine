#include "pch.h"
#include "ResourceManager.h"


ResourceManager::ResourceManager()
{
	m_pInstance = std::make_unique<ResourceManager>(*this);
}

ResourceManager* ResourceManager::GetInstance()
{
	return m_pInstance.get();
}
