#include "render/mesh_factory.h"

namespace sigma {

MeshHandle MeshFactory::createQuad() {
  std::vector<SimpleVertex> vertices = {
      {glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
      {glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
      {glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(1.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
  };
  std::vector<uint32_t> indices = {0, 2, 1, 2, 3, 1};

  MeshHandle mesh = createRef<Mesh>(PrimitiveTopology::TRIANGLE, indices.size());
  mesh->addVertexBuffer(VertexBuffer(vertices, SimpleVertex::getLayout()));
  mesh->setIndexBuffer(IndexBuffer(indices));

  return mesh;
}

MeshHandle MeshFactory::createCube() {
  std::vector<uint32_t> indices = {
      0, 2, 1, 2, 0, 3,
      4, 5, 6, 6, 7, 4,
      8, 9, 10, 10, 11, 8,
      12, 14, 13, 14, 12, 15,
      16, 17, 18, 18, 19, 16,
      20, 22, 21, 22, 20, 23,
  };
  std::vector<SimpleVertex> vertices = {
      {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 0.0f)},
      {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 0.0f)},
      {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},

      {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)},
      {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)},
      {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 1.0f)},

      {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)},
      {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
      {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)},

      {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)},
      {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
      {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)},

      {glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
      {glm::vec3(0.5f, -0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(0.5f, -0.5f, 0.5f), glm::vec2(1.0f, 0.0f)},
      {glm::vec3(-0.5f, -0.5f, 0.5f), glm::vec2(0.0f, 0.0f)},

      {glm::vec3(-0.5f, 0.5f, -0.5f), glm::vec2(0.0f, 1.0f)},
      {glm::vec3(0.5f, 0.5f, -0.5f), glm::vec2(1.0f, 1.0f)},
      {glm::vec3(0.5f, 0.5f, 0.5f), glm::vec2(1.0f, 0.0f)},
      {glm::vec3(-0.5f, 0.5f, 0.5f), glm::vec2(0.0f, 0.0f)},
  };

  MeshHandle mesh = createRef<Mesh>(PrimitiveTopology::TRIANGLE, indices.size());
  mesh->
      addVertexBuffer(VertexBuffer(vertices, SimpleVertex::getLayout())
  );
  mesh->
      setIndexBuffer(IndexBuffer(indices)
  );

  return
      mesh;
}

}
