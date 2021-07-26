#include <glm/gtc/type_ptr.hpp>
#include "render/shader.h"

namespace sigma {

ShaderModule::ShaderModule(const char* src, ShaderStage stage) noexcept
    : id_{glCreateShader(static_cast<GLenum>(stage))}, stage_{stage} {
  glShaderSource(id_, 1, &src, nullptr);
  glCompileShader(id_);
  checkCompileErrors();
}

ShaderModule::~ShaderModule() {
  glDeleteShader(id_);
  id_ = 0;
}

ShaderModule::ShaderModule(ShaderModule&& other) noexcept
    : id_{other.id_}, stage_{other.stage_} {
  other.id_ = 0;
}

ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept {
  if (this != &other) {
    glDeleteShader(id_);
    id_ = 0;
    std::swap(id_, other.id_);
    std::swap(stage_, other.stage_);
  }
  return *this;
}

void ShaderModule::checkCompileErrors() {
  int success;
  glGetShaderiv(id_, GL_COMPILE_STATUS, &success);

  if (!success) {
    constexpr int max_log_size = 512;
    char info_log[max_log_size];
    glGetShaderInfoLog(id_, max_log_size, nullptr,
                       static_cast<char*>(info_log));
    std::cerr << "ShaderModule compilation error: " << id_ << " " << info_log
              << '\n';
  }
}

Shader::Shader(const std::vector<ShaderModule>& modules) noexcept
    : id_{glCreateProgram()} {
  for (const auto& module: modules) {
    glAttachShader(id_, module.id_);
  }
  glLinkProgram(id_);
  checkLinkingErrors();
}

Shader::Shader(Shader&& other) noexcept
    : id_{std::exchange(id_, 0)} {

}

Shader& Shader::operator=(Shader&& other) noexcept {
  if (this != &other) {
    glDeleteProgram(id_);
    id_ = 0;
    std::swap(id_, other.id_);
  }
  return *this;
}

Shader::~Shader() {
  glDeleteProgram(id_);
  id_ = 0;
}

void Shader::checkLinkingErrors() {
  int success;
  glGetProgramiv(id_, GL_LINK_STATUS, &success);
  GLchar info_log[1024];
  if (!success) {
    glGetProgramInfoLog(id_, 1024, nullptr, static_cast<GLchar*>(info_log));
    std::cerr << "Shader linking error:" << info_log << std::endl;
  }
}

void Shader::bind() const {
  glUseProgram(id_);
}

void Shader::unbind() const {
  glUseProgram(0);
}

void Shader::setMat4(const char* name, const glm::mat4& value) const {
  glUniformMatrix4fv(glGetUniformLocation(id_, name), 1, false, glm::value_ptr(value));
}

void Shader::setInt(const char* name, int value) const {
  glUniform1i(glGetUniformLocation(id_, name), value);
}

void Shader::setVec3(const char* name, const glm::vec3& value) const {
  glUniform3f(glGetUniformLocation(id_, name), value.x, value.y, value.z);
}

void Shader::setFloat(const char* name, float value) const {
  glUniform1f(glGetUniformLocation(id_, name), value);
}

ShaderBuilder& ShaderBuilder::loadModule(const std::filesystem::path& path,
                                         ShaderStage stage) {
  std::string source = loadFile(path);

  modules_.emplace_back(source.c_str(), stage);

  return *this;
}

ShaderHandle ShaderBuilder::build() const { return createRef<Shader>(modules_); }

}// namespace sigma
