
#pragma once

#include "core/system.h"
#include "mesh.h"
#include "mesh_factory.h"
#include "texture.h"
#include "frame_buffer.h"

namespace sigma {

struct MeshComponent {
  MeshHandle mesh;
};

struct Material {
  ShaderHandle shader;
  TextureHandle diffuse;
};

class RenderSystem : public System {

 public:
  void start(World& world) override {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    auto window = world.raw().try_ctx<Window>();
    if (!window) {
      return;
    }

    FrameBuffer::Descriptor descriptor{};
    descriptor.width = window->getWidth();
    descriptor.height = window->getHeight();
    descriptor.color_formats = {GL_RGBA};
    descriptor.depth_format = GL_DEPTH_COMPONENT16;
    frame_buffer_ = FrameBuffer(descriptor);

    screen_quad_ = MeshFactory::createQuad();
    screen_shader_ = ShaderBuilder()
        .loadModule("../assets/shaders/screen.vert", ShaderStage::VERTEX)
        .loadModule("../assets/shaders/screen.frag", ShaderStage::FRAGMENT)
        .build();
  }

  void update(World& world) override {
    frame_buffer_.bind();
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
      material.diffuse->bind(0);

      mesh.mesh->bind();
      if (mesh.mesh->isIndexed()) {
        glDrawElements((GLenum)mesh.mesh->getTopology(), mesh.mesh->count(), GL_UNSIGNED_INT, nullptr);
      } else {
        glDrawArrays((GLenum)mesh.mesh->getTopology(), 0, mesh.mesh->count());
      }
    });

    frame_buffer_.unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    screen_shader_->bind();
    frame_buffer_.getTexture(0)->bind(0);
    screen_quad_->bind();
    glDrawElements(GL_TRIANGLES, screen_quad_->count(), GL_UNSIGNED_INT, nullptr);
  }

 private:
  FrameBuffer frame_buffer_;
  MeshHandle screen_quad_;
  ShaderHandle screen_shader_;
};

}  // namespace sigma
