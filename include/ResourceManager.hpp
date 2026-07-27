#ifndef RESOURCE_MANAGER_HPP
#define RESOURCE_MANAGER_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include <stdexcept>
#include <iostream>

template <typename T>
class ResourceManager
{
private:
    std::map<std::string, T> resources;

public:
    T &load(const std::string &name, const std::string &filename)
    {
        if (resources.find(name) == resources.end())
        {
            T resource;
            if (!resource.loadFromFile(filename))
            {
                throw std::runtime_error("Failed to load resource: " + filename);
            }
            resources[name] = std::move(resource);
            std::cout << "[LOG] Loaded resource: " << name << " from " << filename << std::endl;
        }
        return resources[name];
    }

    T &get(const std::string &name)
    {
        if (resources.find(name) != resources.end())
        {
            return resources.at(name);
        }
        throw std::runtime_error("Resource not found: " + name);
    }
};

#endif