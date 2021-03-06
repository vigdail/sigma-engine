#pragma once

#include <glad.h>
#include <optional>
#include "buffer.h"

namespace sigma {

enum class PrimitiveTopology {
  POINTS = GL_POINTS,
  LINES = GL_LINES,
  LINE_STRIP = GL_LINE_STRIP,
  TRIANGLE = GL_TRIANGLES,
  TRIANGLE_STRIP = GL_TRIANGLE_STRIP,
  TRIANGLE_FAN = GL_TRIANGLE_FAN,
  PATCHES = GL_PATCHES,
};

class Mesh {
 public:
  Mesh(PrimitiveTopology topology, size_t count);
  Mesh(Mesh&& other) noexcept;
  Mesh& operator=(Mesh&& other) noexcept;
  Mesh(const Mesh& other) = delete;
  Mesh& operator=(const Mesh& other) = delete;
  ~Mesh();
  void bind() const;
  void unbind() const;
  void addVertexBuffer(Buffer&& buffer, const BufferLayout& layout);
  void setIndexBuffer(Buffer&& buffer);

  bool isIndexed() const { return is_indexed_; }

  void setCount(uint32_t count) { count_ = count; }

  size_t count() const {
    return count_;
  }

  PrimitiveTopology getTopology() const;

  void setTopology(PrimitiveTopology topology) { topology_ = topology; }

 private:
  uint32_t id_{};
  size_t count_;
  bool is_indexed_{false};
  PrimitiveTopology topology_;
  std::vector<Buffer> buffers_;
};

using MeshHandle = Ref<Mesh>;

}
