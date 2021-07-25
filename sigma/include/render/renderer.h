
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
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
  }

  void update(World& world) override {
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
      shader->setMat4("model", transform.transform());

      mesh.mesh->bind();
      if (mesh.mesh->isIndexed()) {
        glDrawElements((GLenum)mesh.mesh->getTopology(), mesh.mesh->count(), GL_UNSIGNED_INT, nullptr);
      } else {
        glDrawArrays((GLenum)mesh.mesh->getTopology(), 0, mesh.mesh->count());
      }
    });
  }

};

}  // namespace sigma
