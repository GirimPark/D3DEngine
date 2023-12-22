#pragma once
#include <map>
#include <memory>
#include <string>

class Material;

class ResourceManager
{
private:
	static std::unique_ptr<ResourceManager> m_pInstance;

	std::map<std::string, std::weak_ptr<Material>> m_materialMap;

private:
	ResourceManager();

public:
	ResourceManager* GetInstance();
};

