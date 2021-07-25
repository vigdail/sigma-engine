#pragma once

#include <glad/glad.h>
#include "buffer.h"

namespace sigma {

enum class VertexFormat {
  FLOAT = GL_FLOAT,
};

struct VertexAttribute {
  VertexFormat type = VertexFormat::FLOAT;
  std::size_t count;
  std::size_t offset;
  uint32_t location;
  bool normalized;
};

struct BufferLayout {
  std::size_t stride;
  std::vector<VertexAttribute> attributes;
};

struct SimpleVertex {
  glm::vec3 position;
  glm::vec2 uv;

  static BufferLayout getLayout() {
    VertexAttribute position_attr{};
    position_attr.count = 3;
    position_attr.offset = 0;
    position_attr.location = 0;
    position_attr.normalized = false;
    VertexAttribute uv_attr{};
    uv_attr.count = 2;
    uv_attr.offset = offsetof(SimpleVertex, uv);
    uv_attr.location = 1;
    uv_attr.normalized = false;

    BufferLayout layout{};
    layout.stride = sizeof(SimpleVertex);
    layout.attributes = {position_attr, uv_attr};

    return layout;
  }
};

}