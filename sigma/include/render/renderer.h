
#pragma once

#include "core/system.h"
#include "mesh.h"
#include "mesh_factory.h"

namespace sigma {

struct MeshComponent {
  MeshHandle mesh;
};

struct Material {
  ShaderHandle shader;
};

class RenderSystem : public System {

 public:
  void start(World& world) override {
  }

  void update(World& world) override {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto cameras = world.raw().view<Camera>();
    if (cameras.size() < 1) {
      return;
    }

    const auto& camera = world.raw().get<Camera>(cameras[0]);

    auto view = world.raw().view<MeshComponent, Transform, Material>();
    view.each([&](const auto& mesh, const auto& transform, const auto& material) {
      auto& shader = material.shader;
      shader->bind();
      shader->setMat4("view", camera.view);
      shader->setMat4("proj", camera.projection);
      glm::mat4 model(1.0f);
      model = glm::translate(model, glm::vec3(transform.x, transform.y, 0.0f));
      model = glm::scale(model, glm::vec3(transform.width, transform.height, 1.0f));
      shader->setMat4("model", model);

      mesh.mesh->bind();
      glDrawElements((GLenum)mesh.mesh->getTopology(), mesh.mesh->count(), GL_UNSIGNED_INT, nullptr);
    });
  }

};

}  // namespace sigma
