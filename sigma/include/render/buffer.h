#pragma once

#include <util/non_copyable.h>
#include <glad.h>
#include "vertex.h"

namespace sigma {

enum class BufferType {
  VERTEX = GL_ARRAY_BUFFER,
  INDEX = GL_ELEMENT_ARRAY_BUFFER,
  UNIFORM = GL_UNIFORM_BUFFER,
};

enum class BufferUsage {
  STREAM_DRAW = GL_STREAM_DRAW,
  STREAM_READ = GL_STREAM_READ,
  STREAM_COPY = GL_STREAM_COPY,
  STATIC_DRAW = GL_STATIC_DRAW,
  STATIC_READ = GL_STATIC_READ,
  STATIC_COPY = GL_STATIC_COPY,
  DYNAMIC_DRAW = GL_DYNAMIC_DRAW,
  DYNAMIC_READ = GL_DYNAMIC_READ,
  DYNAMIC_COPY = GL_DYNAMIC_COPY,
};

class Buffer final : public NonCopyable {
 public:
  Buffer(BufferType type, std::size_t size, BufferUsage usage) noexcept;
  Buffer(BufferType type, const uint8_t* data, std::size_t size, BufferUsage usage = BufferUsage::STATIC_DRAW) noexcept;
  Buffer(Buffer&& other) noexcept;
  Buffer& operator=(Buffer&& other) noexcept;
  ~Buffer();
  void drop();
  void setSubData(const uint8_t* data, std::size_t size, std::size_t offset);
  void bind() const;
  void unbind() const;
  void bindBase(std::size_t index) const;

 private:
  uint32_t id_;
  BufferType type_{BufferType::VERTEX};
  BufferUsage usage_{BufferUsage::STATIC_DRAW};
  std::size_t size_{0};
};

}

