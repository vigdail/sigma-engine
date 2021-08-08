#pragma once

#include <glad/glad.h>
#include <stb_image/stb_image.h>

#include "resources/resource_manager.h"

#include <iostream>
#include <filesystem>

namespace sigma {

struct TextureSamplerDescriptor {
  uint32_t filter_min = GL_LINEAR;
  uint32_t filter_mag = GL_LINEAR;
  uint32_t wrap_s = GL_CLAMP_TO_EDGE;
  uint32_t wrap_t = GL_CLAMP_TO_EDGE;
  uint32_t wrap_r = GL_CLAMP_TO_EDGE;
};

struct TextureViewDescriptor {
  uint32_t width = 0;
  uint32_t height = 0;
  uint32_t internal_format = GL_RGB;
  uint32_t image_format = GL_RGB;
  uint32_t type = GL_UNSIGNED_BYTE;
};

class TextureBuilder;

class Texture final {
  friend TextureBuilder;

 public:
  Texture() noexcept;
  Texture(const Texture&) = delete;
  Texture(Texture&& other) noexcept;
  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&& other) noexcept;
  ~Texture();
  void bind(uint32_t unit = 0) const;
  bool loadFromFile(const std::filesystem::path& path);

  uint32_t getId() const { return id_; }

  uint32_t getWidth() const { return view_.width; }

  uint32_t getHeight() const { return view_.height; }

 private:
  uint32_t id_{};
  TextureViewDescriptor view_;
  TextureSamplerDescriptor sampler_;

 private:
  void drop();
};

using TextureHandle = AssetHandle<Texture>;

class TextureBuilder final {
 public:
  TextureBuilder() noexcept = default;
  TextureBuilder& withSampler(const TextureSamplerDescriptor& sampler);
  TextureBuilder& withView(const TextureViewDescriptor& view);
  TextureBuilder& withData(unsigned char* data);
  TextureBuilder& load(std::string_view path);
  Ref<Texture> build() const;

 private:
  TextureSamplerDescriptor sampler_;
  TextureViewDescriptor view_;
  unsigned char* data_ = nullptr;
  bool loaded_ = false;
};

}
