#include "render/mesh.h"

namespace sigma {

Mesh::Mesh(PrimitiveTopology topology, size_t count)
    : count_{count}, topology_{topology} {
  glCreateVertexArrays(1, &id_);
}

Mesh::Mesh(Mesh&& other) noexcept
    : id_{other.id_},
      count_{other.count_},
      topology_{other.topology_},
      is_indexed_{other.is_indexed_} {
  other.id_ = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
  std::swap(id_, other.id_);
  std::swap(count_, other.count_);
  std::swap(topology_, other.topology_);
  std::swap(is_indexed_, other.is_indexed_);
  return *this;
}

Mesh::~Mesh() {
  glDeleteVertexArrays(1, &id_);
  id_ = 0;
}

void Mesh::bind() const { glBindVertexArray(id_); }

void Mesh::unbind() const { glBindVertexArray(0); }

PrimitiveTopology Mesh::getTopology() const {
  return topology_;
}

void Mesh::addVertexBuffer(Buffer&& buffer, const BufferLayout& layout) {
  glBindVertexArray(id_);
  buffer.bind();

  for (const auto& element : layout.attributes) {
    glEnableVertexAttribArray(element.location);
    glVertexAttribPointer(element.location, element.count,
                          static_cast<GLenum>(element.type),
                          element.normalized ? GL_TRUE : GL_FALSE,
                          layout.stride, reinterpret_cast<const void*>(element.offset));
  }

  buffers_.emplace_back(std::move(buffer));
}

void Mesh::setIndexBuffer(Buffer&& buffer) {
  glBindVertexArray(id_);
  buffer.bind();
  is_indexed_ = true;
  buffers_.emplace_back(std::move(buffer));
}

}