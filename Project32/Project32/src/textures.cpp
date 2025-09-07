#include "common.h"
#include "textures.h"

void Texture::Bind(GLenum textureUnit) const {
	glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_2D, _textureID);
}

bool Texture::LoadFromFile(const std::string& textureName, const std::string& filepath, bool flipVertically) {
	_name = textureName;

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