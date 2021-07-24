#pragma once

#include <filesystem>
#include <glad.h>
#include <util/non_copyable.h>
#include <util/util.h>

namespace sigma {

enum class ShaderStage {
  VERTEX = GL_VERTEX_SHADER,
  FRAGMENT = GL_FRAGMENT_SHADER,
};

class Shader;

class ShaderModule final : public NonCopyable {

  friend Shader;

 public:
  ShaderModule() = default;
  ShaderModule(const char* source, ShaderStage stage) noexcept;
  ShaderModule(ShaderModule&& other) noexcept;
  ShaderModule& operator=(ShaderModule&& other) noexcept;
  ~ShaderModule();

 private:
  uint32_t id_;
  ShaderStage stage_;

 private:
  void checkCompileErrors();
};

class Shader final : public NonCopyable {
 public:
  explicit Shader(const std::vector<ShaderModule>& modules) noexcept;
  Shader(Shader&& other) noexcept;
  Shader& operator=(Shader&& other) noexcept;
  ~Shader();

 private:
  uint32_t id_;

 private:
  void checkLinkingErrors();
};

class ShaderBuilder {

 public:
  ShaderBuilder& loadModule(const std::filesystem::path& path, ShaderStage stage);
  Shader build() const;

 private:
  std::vector<ShaderModule> modules_{};
};
}// namespace sigma