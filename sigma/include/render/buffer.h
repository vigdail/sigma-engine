#pragma once

#include <util/non_copyable.h>
#include <glad.h>
#include "vertex.h"

namespace sigma {

class VertexBuffer final : public NonCopyable {
 public:
  explicit VertexBuffer(std::size_t size) noexcept;

  template<typename Vertex>
  VertexBuffer(const std::vector<Vertex>& vertices, BufferLayout layout) noexcept
      : layout_{std::move(layout)}, size_{vertices.size() * sizeof(Vertex)} {
    glCreateBuffers(1, &id_);
    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex),
                 vertices.data(), GL_STATIC_DRAW);
  }

  ~VertexBuffer();

  VertexBuffer(VertexBuffer&& other) noexcept;
  VertexBuffer& operator=(VertexBuffer&& other) noexcept;

  template<typename Vertex>
  void setData(const std::vector<Vertex>& vertices,
               const BufferLayout& layout) {
    assert(vertices.size() * sizeof(Vertex) <= size_);

    glBindBuffer(GL_ARRAY_BUFFER, id_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
  }

  void bind() const;
  void unbind() const;
  BufferLayout getLayout() const;

 private:
  uint32_t id_{};
  BufferLayout layout_{};
  std::size_t size_{};
};

class IndexBuffer final : public NonCopyable {
 public:
  explicit IndexBuffer(const std::vector<uint32_t>& indices) noexcept;
  IndexBuffer(IndexBuffer&& other) noexcept;
  IndexBuffer& operator=(IndexBuffer&& other) noexcept;
  ~IndexBuffer();
  void bind() const;
  void unbind() const;
  std::size_t count() const;

 private:
  uint32_t id_{};
  std::size_t count_{};
};

class UniformBuffer final : public NonCopyable {
 public:
  explicit UniformBuffer(std::size_t size) noexcept {
    glCreateBuffers(1, &id_);
    glBindBuffer(GL_UNIFORM_BUFFER, id_);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

  ~UniformBuffer() {
    glDeleteBuffers(1, &id_);
    id_ = 0;
  }

  void bindBase(int binding) const {
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, id_);
  }

  void setData(const void* data, std::size_t start, std::size_t size) {
    bind();
    glBufferSubData(GL_UNIFORM_BUFFER, start, size, data);
    unbind();
  }

  void bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, id_);
  }

  void unbind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
  }

 private:
  uint32_t id_{};
};

}

