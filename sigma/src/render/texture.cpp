#include "render/texture.h"
#include <iostream>
#include <memory>
#include <utility>

namespace sigma {

Texture::Texture() noexcept: view_{} { glGenTextures(1, &id_); }

Texture::Texture(Texture&& other) noexcept
    : id_(std::exchange(other.id_, 0)), view_{other.view_} {}

Texture& Texture::operator=(Texture&& other) noexcept {
  if (this != &other) {
    drop();
    std::swap(id_, other.id_);
    std::swap(view_, other.view_);
  }

  return *this;
}

Texture::~Texture() { drop(); }

void Texture::drop() {
  glDeleteTextures(1, &id_);
  id_ = 0;
}

void Texture::bind(uint32_t unit) const {
  glBindTextureUnit(unit, id_);
}

bool Texture::loadFromFile(const std::filesystem::path& path) {
  int width;
  int height;
  int n_channels;
  unsigned char* data = stbi_load(path.c_str(), &width, &height, &n_channels, 0);

  if (!data) {
    std::cerr << "ERROR::TEXTURE: Unable to load texture from file: " << path
              << std::endl;
//    throw std::runtime_error("Failed to load texture from file");
    return false;
  }

  view_.width = width;
  view_.height = height;

  if (n_channels == 1) {
    view_.image_format = GL_RED;
    view_.internal_format = GL_RED;
  } else if (n_channels == 3) {
    view_.image_format = GL_RGB;
    view_.internal_format = GL_RGB;
  } else if (n_channels == 4) {
    view_.image_format = GL_RGBA;
    view_.internal_format = GL_RGBA;
  }

  glBindTexture(GL_TEXTURE_2D, id_);
  glTexImage2D(GL_TEXTURE_2D, 0, view_.internal_format, view_.width,
               view_.height, 0, view_.image_format, view_.type, data);
  glBindTexture(GL_TEXTURE_2D, 0);

  glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, sampler_.filter_min);
  glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, sampler_.filter_mag);
  glTextureParameteri(id_, GL_TEXTURE_WRAP_S, sampler_.wrap_s);
  glTextureParameteri(id_, GL_TEXTURE_WRAP_T, sampler_.wrap_t);

  stbi_image_free(data);

  return true;
}

TextureBuilder& TextureBuilder::withSampler(
    const TextureSamplerDescriptor& sampler) {
  sampler_ = sampler;
  return *this;
}

TextureBuilder& TextureBuilder::withView(const TextureViewDescriptor& view) {
  view_ = view;
  return *this;
}

TextureBuilder& TextureBuilder::load(std::string_view path) {
  if (loaded_ && data_) {
    stbi_image_free(data_);
  }

  int width;
  int height;
  int n_channels;
  data_ = stbi_load(path.data(), &width, &height, &n_channels, 0);

  if (!data_) {
    std::cerr << "ERROR::TEXTURE: Unable to load texture from file: " << path
              << std::endl;
    throw std::runtime_error("Failed to load texture from file");
  }

  view_.width = width;
  view_.height = height;

  if (n_channels == 1) {
    view_.image_format = GL_RED;
    view_.internal_format = GL_RED;
  } else if (n_channels == 3) {
    view_.image_format = GL_RGB;
    view_.internal_format = GL_RGB;
  } else if (n_channels == 4) {
    view_.image_format = GL_RGBA;
    view_.internal_format = GL_RGBA;
  }

  loaded_ = true;

  return *this;
}

Ref<Texture> TextureBuilder::build() const {
  Ref<Texture> texture = createRef<Texture>();

  texture->view_ = view_;
  glBindTexture(GL_TEXTURE_2D, texture->id_);
  glTexImage2D(GL_TEXTURE_2D, 0, view_.internal_format, view_.width,
               view_.height, 0, view_.image_format, view_.type, data_);
  glBindTexture(GL_TEXTURE_2D, 0);

  glTextureParameteri(texture->id_, GL_TEXTURE_MIN_FILTER, sampler_.filter_min);
  glTextureParameteri(texture->id_, GL_TEXTURE_MAG_FILTER, sampler_.filter_mag);
  glTextureParameteri(texture->id_, GL_TEXTURE_WRAP_S, sampler_.wrap_s);
  glTextureParameteri(texture->id_, GL_TEXTURE_WRAP_T, sampler_.wrap_t);

  if (loaded_ && data_) {
    stbi_image_free(data_);
  }

  return texture;
}

TextureBuilder& TextureBuilder::withData(unsigned char* data) {
  if (loaded_ && data_) {
    stbi_image_free(data_);
  }

  data_ = data;

  return *this;
}

}