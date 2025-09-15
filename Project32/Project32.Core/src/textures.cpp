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

#include "common.h"
#include "textures.h"

// Texture functions
void Texture::Bind(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, _textureID);
}

bool Texture::LoadFromFile(const std::string& textureName, const std::string& filepath, bool flipVertically) {
	_name = textureName;
	_filepath = filepath;

	if (_textureID != 0) {
		glDeleteTextures(1, &_textureID);
		_textureID = 0;
	}
	glGenTextures(1, &_textureID);
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (flipVertically) {
		stbi_set_flip_vertically_on_load(true);
	}
	unsigned char* data = stbi_load(filepath.c_str(), &_width, &_height, &_channels, 0);
	if (data) {
		GLenum format = (_channels == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_2D, 0, format, _width, _height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cerr << "Failed to load texture: " << filepath << std::endl;
		return false;
	}
	stbi_image_free(data);
	if (flipVertically) {
		stbi_set_flip_vertically_on_load(false);
	}

	return true;
}

// TextureManager functions
TextureManager::TextureManager() : _activeTextureIndex(0) {}

int TextureManager::AddTexture(const std::string& name) {
	int index = static_cast<int>(_textures.size());
	_textures.push_back(std::make_unique<Texture>(name));
	return index;
}

int TextureManager::AddExistingTexture(std::unique_ptr<Texture> texture) {
	int index = static_cast<int>(_textures.size());
	_textures.push_back(std::move(texture));
	return index;
}

int TextureManager::LoadTexture(const std::string& name, const std::string& filepath, bool flipVertically = true) {
	auto texture = std::make_unique<Texture>(name);
	if (texture->LoadFromFile(name, filepath, flipVertically)) {
		int index = static_cast<int>(_textures.size());
		_textures.push_back(std::move(texture));
		return index;
	}
	return -1;
}

Texture& TextureManager::GetTexture(int index) {
	if (index >= _textures.size()) {
		throw std::out_of_range("Texture index out of range");
	}
	return *_textures[index];
}

const Texture& TextureManager::GetTexture(int index) const {
	if (index >= _textures.size()) {
		throw std::out_of_range("Texture index out of range");
	}
	return *_textures[index];
}

Texture* TextureManager::GetTextureByName(const std::string& name) {
    for (auto& texture : _textures) {
        if (texture->GetName() == name) {
            return texture.get();
        }
    }
    return nullptr;
}

const Texture* TextureManager::GetTextureByName(const std::string& name) const {
    for (const auto& texture : _textures) {
        if (texture->GetName() == name) {
            return texture.get();
        }
    }
    return nullptr;
}

Texture& TextureManager::GetActiveTexture() {
    if (_textures.empty()) {
        throw std::runtime_error("No textures available");
    }
    return *_textures[_activeTextureIndex];
}

const Texture& TextureManager::GetActiveTexture() const {
    if (_textures.empty()) {
        throw std::runtime_error("No textures available");
    }
    return *_textures[_activeTextureIndex];
}

void TextureManager::SetActiveTexture(int index) {
    if (index >= _textures.size()) {
        throw std::out_of_range("Texture index out of range");
    }
    _activeTextureIndex = index;
}

void TextureManager::SetActiveTextureByName(const std::string& name) {
    int index = FindTextureByName(name);
    _activeTextureIndex = index;
}

void TextureManager::SetToNextTexture() {
    if (_textures.empty()) {
        throw std::runtime_error("No textures available");
    }
    int size = static_cast<int>(_textures.size());
    _activeTextureIndex = (_activeTextureIndex + 1) % size;
}

int TextureManager::FindTextureByName(const std::string& name) const {
    for (int i = 0; i < _textures.size(); ++i) {
        if (_textures[i]->GetName() == name) {
            return i;
        }
    }
    throw std::runtime_error("Texture not found: " + name);
}

void TextureManager::RemoveTexture(int index) {
    int size = static_cast<int>(_textures.size());
    if (index >= size) {
        return;
    }

    _textures.erase(_textures.begin() + index);

    if (_activeTextureIndex >= size - 1 && !_textures.empty()) {
        _activeTextureIndex = size - 2;
    }
    if (_textures.empty()) {
        _activeTextureIndex = 0;
    }
}

bool TextureManager::RemoveTextureByName(const std::string& name) {
    int size = static_cast<int>(_textures.size());
    for (int i = 0; i < size; ++i) {
        if (_textures[i]->GetName() == name) {
            RemoveTexture(i);
            return true;
        }
    }
    return false;
}

int TextureManager::GetTextureCount() const {
    return static_cast<int>(_textures.size());
}

bool TextureManager::HasTextures() const {
    return !_textures.empty();
}

int TextureManager::GetActiveTextureIndex() const {
    return _activeTextureIndex;
}

void TextureManager::BindActiveTexture(GLenum textureUnit = GL_TEXTURE0) const {
    if (!_textures.empty()) {
        _textures[_activeTextureIndex]->Bind(textureUnit);
    }
}

void TextureManager::BindTexture(int index, GLenum textureUnit = GL_TEXTURE0) const {
    if (index < _textures.size()) {
        _textures[index]->Bind(textureUnit);
    }
}

void TextureManager::BindTextureByName(const std::string& name, GLenum textureUnit = GL_TEXTURE0) const {
    const Texture* texture = GetTextureByName(name);
    if (texture) {
        texture->Bind(textureUnit);
    }
}

auto TextureManager::begin() { return _textures.begin(); }
auto TextureManager::end() { return _textures.end(); }
auto TextureManager::begin() const { return _textures.begin(); }
auto TextureManager::end() const { return _textures.end(); }

bool TextureManager::Clear() {
    if (_textures.empty()) {
        return false;
    }
    _textures.clear();
    _activeTextureIndex = 0;
    return true;
}