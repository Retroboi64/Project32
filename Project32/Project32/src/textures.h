#pragma once

#include "common.h"

class Texture {
private:
	GLuint _textureID = 0;
	int _width = 0, _height = 0, _channels = 0;
	std::string _name;
public:
	Texture() = default;
	~Texture() = default;
	bool LoadFromFile(const std::string& textureName, const std::string& filepath, bool flipVertically = true);
	void Bind(GLenum textureUnit = GL_TEXTURE0) const;
	int GetWidth() const { return _width; }
	int GetHeight() const { return _height; }
	int GetChannels() const { return _channels; }
	GLuint GetTextureID() const { return _textureID; }
};