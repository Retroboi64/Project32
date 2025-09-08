#pragma once

#include "common.h"

class Texture {
private:
    GLuint _textureID = 0;
    int _width = 0, _height = 0, _channels = 0;
    std::string _name;
public:
    Texture() = default;
    Texture(const std::string& name) : _name(name) {}
    ~Texture() {
        if (_textureID != 0) {
            glDeleteTextures(1, &_textureID);
        }
    }

    // Disable copy constructor and copy assignment to prevent double deletion
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    // Enable move constructor and move assignment
    Texture(Texture&& other) noexcept
        : _textureID(other._textureID), _width(other._width),
        _height(other._height), _channels(other._channels), _name(std::move(other._name)) {
        other._textureID = 0; // Prevent double deletion
    }

    Texture& operator=(Texture&& other) noexcept {
        if (this != &other) {
            if (_textureID != 0) {
                glDeleteTextures(1, &_textureID);
            }
            _textureID = other._textureID;
            _width = other._width;
            _height = other._height;
            _channels = other._channels;
            _name = std::move(other._name);
            other._textureID = 0;
        }
        return *this;
    }

    bool LoadFromFile(const std::string& textureName, const std::string& filepath, bool flipVertically = true);
    void Bind(GLenum textureUnit = GL_TEXTURE0) const;

    // Getters
    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }
    int GetChannels() const { return _channels; }
    GLuint GetTextureID() const { return _textureID; }
    const std::string& GetName() const { return _name; }

    // Setter for name
    void SetName(const std::string& name) { _name = name; }
};

class TextureManager {
private:
    std::vector<std::unique_ptr<Texture>> _textures;
    int _activeTextureIndex;

public:
    TextureManager() : _activeTextureIndex(0) {}

    // Add texture methods
    int AddTexture(const std::string& name) {
        int index = static_cast<int>(_textures.size());
        _textures.push_back(std::make_unique<Texture>(name));
        return index;
    }

    int AddExistingTexture(std::unique_ptr<Texture> texture) {
        int index = static_cast<int>(_textures.size());
        _textures.push_back(std::move(texture));
        return index;
    }

    // Load texture from file and add to manager
    int LoadTexture(const std::string& name, const std::string& filepath, bool flipVertically = true) {
        auto texture = std::make_unique<Texture>(name);
        if (texture->LoadFromFile(name, filepath, flipVertically)) {
            int index = static_cast<int>(_textures.size());
            _textures.push_back(std::move(texture));
            return index;
        }
        return -1; // Failed to load
    }

    // Get texture methods
    Texture& GetTexture(int index) {
        if (index >= _textures.size()) {
            throw std::out_of_range("Texture index out of range");
        }
        return *_textures[index];
    }

    const Texture& GetTexture(int index) const {
        if (index >= _textures.size()) {
            throw std::out_of_range("Texture index out of range");
        }
        return *_textures[index];
    }

    Texture* GetTextureByName(const std::string& name) {
        for (auto& texture : _textures) {
            if (texture->GetName() == name) {
                return texture.get();
            }
        }
        return nullptr;
    }

    const Texture* GetTextureByName(const std::string& name) const {
        for (const auto& texture : _textures) {
            if (texture->GetName() == name) {
                return texture.get();
            }
        }
        return nullptr;
    }

    // Active texture management
    Texture& GetActiveTexture() {
        if (_textures.empty()) {
            throw std::runtime_error("No textures available");
        }
        return *_textures[_activeTextureIndex];
    }

    const Texture& GetActiveTexture() const {
        if (_textures.empty()) {
            throw std::runtime_error("No textures available");
        }
        return *_textures[_activeTextureIndex];
    }

    void SetActiveTexture(int index) {
        if (index >= _textures.size()) {
            throw std::out_of_range("Texture index out of range");
        }
        _activeTextureIndex = index;
    }

    void SetActiveTextureByName(const std::string& name) {
        int index = FindTextureByName(name);
        _activeTextureIndex = index;
    }

    void SetToNextTexture() {
        if (_textures.empty()) {
            throw std::runtime_error("No textures available");
        }
        int size = static_cast<int>(_textures.size());
        _activeTextureIndex = (_activeTextureIndex + 1) % size;
    }

    // Find texture
    int FindTextureByName(const std::string& name) const {
        for (int i = 0; i < _textures.size(); ++i) {
            if (_textures[i]->GetName() == name) {
                return i;
            }
        }
        throw std::runtime_error("Texture not found: " + name);
    }

    // Remove texture methods
    void RemoveTexture(int index) {
        int size = static_cast<int>(_textures.size());
        if (index >= size) {
            return;
        }

        _textures.erase(_textures.begin() + index);

        // Adjust active texture index if necessary
        if (_activeTextureIndex >= size - 1 && !_textures.empty()) {
            _activeTextureIndex = size - 2; // size - 1 after removal
        }
        if (_textures.empty()) {
            _activeTextureIndex = 0;
        }
    }

    bool RemoveTextureByName(const std::string& name) {
        int size = static_cast<int>(_textures.size());
        for (int i = 0; i < size; ++i) {
            if (_textures[i]->GetName() == name) {
                RemoveTexture(i);
                return true;
            }
        }
        return false;
    }

    int GetTextureCount() const {
        return static_cast<int>(_textures.size());
    }

    bool HasTextures() const {
        return !_textures.empty();
    }

    int GetActiveTextureIndex() const {
        return _activeTextureIndex;
    }

    void BindActiveTexture(GLenum textureUnit = GL_TEXTURE0) const {
        if (!_textures.empty()) {
            _textures[_activeTextureIndex]->Bind(textureUnit);
        }
    }

    void BindTexture(int index, GLenum textureUnit = GL_TEXTURE0) const {
        if (index < _textures.size()) {
            _textures[index]->Bind(textureUnit);
        }
    }

    void BindTextureByName(const std::string& name, GLenum textureUnit = GL_TEXTURE0) const {
        const Texture* texture = GetTextureByName(name);
        if (texture) {
            texture->Bind(textureUnit);
        }
    }

    auto begin() { return _textures.begin(); }
    auto end() { return _textures.end(); }
    auto begin() const { return _textures.begin(); }
    auto end() const { return _textures.end(); }

    bool Clear () {
        if (_textures.empty()) {
            return false; 
        }
        _textures.clear();
        _activeTextureIndex = 0;
        return true; 
    }
};