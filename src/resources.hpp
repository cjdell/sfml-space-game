#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include <stdexcept>

class ResourceManager
{
public:
    static ResourceManager &getInstance()
    {
        static ResourceManager instance;
        return instance;
    }

    void loadResource(const std::string &name, const std::string &filePath)
    {
        // Load texture
        sf::Texture texture;
        if (!texture.loadFromFile(filePath))
        {
            throw std::runtime_error("Failed to load texture: " + filePath);
        }
        texture.setSmooth(true);
        _textures[name] = std::move(texture);

        // Load image
        sf::Image image;
        if (!image.loadFromFile(filePath))
        {
            throw std::runtime_error("Failed to load image: " + filePath);
        }
        _images[name] = std::move(image);
    }

    const sf::Texture &getTexture(const std::string &name) const
    {
        auto it = _textures.find(name);
        if (it == _textures.end())
        {
            throw std::runtime_error("Texture not found: " + name);
        }
        return it->second;
    }

    const sf::Image &getImage(const std::string &name) const
    {
        auto it = _images.find(name);
        if (it == _images.end())
        {
            throw std::runtime_error("Image not found: " + name);
        }
        return it->second;
    }

private:
    ResourceManager() = default;
    std::unordered_map<std::string, sf::Texture> _textures;
    std::unordered_map<std::string, sf::Image> _images;
};
