#include "render/mesh_factory.h"

namespace sigma {

MeshHandle MeshFactory::createQuad() {
  std::vector<SimpleVertex> vertices = {
      {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
      {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
      {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
  };
  std::vector<uint32_t> indices = {0, 1, 2, 2, 1, 3};

  MeshHandle mesh = createRef<Mesh>(PrimitiveTopology::TRIANGLE, indices.size());
  mesh->addVertexBuffer(VertexBuffer(vertices, SimpleVertex::getLayout()));
  mesh->setIndexBuffer(IndexBuffer(indices));

  return mesh;
}

MeshHandle MeshFactory::createCube() {
  std::vector<SimpleVertex> vertices = {};
  std::vector<uint32_t> indices = {};

  MeshHandle mesh = createRef<Mesh>(PrimitiveTopology::TRIANGLE, indices.size());
  mesh->addVertexBuffer(VertexBuffer(vertices, SimpleVertex::getLayout()));
  mesh->setIndexBuffer(IndexBuffer(indices));

  return mesh;
}

}
