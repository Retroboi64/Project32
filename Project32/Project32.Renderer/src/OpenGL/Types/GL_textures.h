/*
 * This file is part of Project32 - A compact yet powerful and flexible C++ Game Engine
 * Copyright (c) 2025 Patrick Reese (Retroboi64)
 *
 * Licensed under MIT with Attribution Requirements
 * See LICENSE file for full terms
 * GitHub: https://github.com/Retroboi64/Project32
 *
 * This header must not be removed from any source file.
 */

#ifndef TEXTURE_H
#define TEXTURE_H

#include "../../r_common.h"

class Texture {
private:
    GLuint _textureID = 0;
    int _width = 0, _height = 0, _channels = 0;
    std::string _name;
    std::string _filepath;

public:
    Texture() = default;
    Texture(const std::string& name) : _name(name) {}
    ~Texture() {
        if (_textureID != 0) {
            glDeleteTextures(1, &_textureID);
        }
    }

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

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

    int GetWidth() const { return _width; }
    int GetHeight() const { return _height; }
    int GetChannels() const { return _channels; }
    std::string GetFilepath() const { return _filepath; }
    GLuint GetTextureID() const { return _textureID; }
    const std::string& GetName() const { return _name; }

    void SetName(const std::string& name) { _name = name; }
};

class TextureManager {
private:
    std::vector<std::unique_ptr<Texture>> _textures;
    int _activeTextureIndex;

public:
    TextureManager();

    // TODO: Organize functions better
    int AddTexture(const std::string& name);
    int AddExistingTexture(std::unique_ptr<Texture> texture);
    int LoadTexture(const std::string& name, const std::string& filepath, bool flipVertically);
    Texture& GetTexture(int index);
    const Texture& GetTexture(int index) const;
    Texture* GetTextureByName(const std::string& name);
    const Texture* GetTextureByName(const std::string& name) const;
    Texture& GetActiveTexture();
    const Texture& GetActiveTexture() const;
    void SetActiveTexture(int index);
    void SetActiveTextureByName(const std::string& name);
    void SetToNextTexture();
    int FindTextureByName(const std::string& name) const;
    void RemoveTexture(int index);
    bool RemoveTextureByName(const std::string& name);
    int GetTextureCount() const;
    bool HasTextures() const;
    int GetActiveTextureIndex() const;
    void BindActiveTexture(GLenum textureUnit) const;
    void BindTexture(int index, GLenum textureUnit) const;
    void BindTextureByName(const std::string& name, GLenum textureUnit) const;
    auto begin();
    auto end();
    auto begin() const;
    auto end() const;
    bool Clear();
};

#endif // !TEXTURE_H