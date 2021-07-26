
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

struct PbrMaterial {
  ShaderHandle shader;
  TextureHandle albedo;
  TextureHandle normalmap;
  TextureHandle metallic;
  TextureHandle roughness;
  TextureHandle ao;
  TextureHandle emission;
};

struct DummyTextures {
  TextureHandle white;
  TextureHandle black;
  TextureHandle red;
  TextureHandle normal;
};

class RenderSystem : public System {

 public:
  void start(World& world) override {
    glClearColor(0.1f, 0.2f, 0.3f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    createDummyTextures(world);

    auto window = world.raw().try_ctx<Window>();
    if (!window) {
      return;
    }

    FrameBuffer::Descriptor descriptor{};
    descriptor.width = window->getWidth();
    descriptor.height = window->getHeight();
    descriptor.color_formats = {GL_RGBA16F, GL_RGBA16F, GL_RGBA, GL_RGB};
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

    auto view = world.raw().view<MeshComponent, Transform, PbrMaterial>();
    view.each([&](const auto& mesh, const auto& transform, const auto& material) {
      auto& shader = material.shader;
      shader->bind();
      shader->setMat4("view", camera.view);
      shader->setMat4("projection", camera.projection);
      shader->setMat4("model", transform.transform());
      shader->setInt("albedo", 0);
      shader->setInt("normalmap", 1);
      shader->setInt("metallic", 2);
      shader->setInt("roughness", 3);
      shader->setInt("ao", 4);
      shader->setInt("emissionmap", 5);

      bindTextures(material, world);

      mesh.mesh->bind();
      if (mesh.mesh->isIndexed()) {
        glDrawElements((GLenum)mesh.mesh->getTopology(), mesh.mesh->count(), GL_UNSIGNED_INT, nullptr);
      } else {
        glDrawArrays((GLenum)mesh.mesh->getTopology(), 0, mesh.mesh->count());
      }
    });

    drawScreenQuad();
  }

 private:
  FrameBuffer frame_buffer_;
  MeshHandle screen_quad_;
  ShaderHandle screen_shader_;

 private:
  void bindTextures(const PbrMaterial& material, const World& world) {
    const auto& dummies = world.resource<DummyTextures>();
    if (material.albedo) {
      material.albedo->bind(0);
    } else {
      dummies.white->bind(0);
    }
    if (material.normalmap) {
      material.normalmap->bind(1);
    } else {
      dummies.normal->bind(1);
    }
    if (material.metallic) {
      material.metallic->bind(2);
    } else {
      dummies.red->bind(2);
    }
    if (material.roughness) {
      material.roughness->bind(3);
    } else {
      dummies.red->bind(3);
    }
    if (material.ao) {
      material.ao->bind(4);
    } else {
      dummies.red->bind(4);
    }
    if (material.emission) {
      material.emission->bind(5);
    } else {
      dummies.black->bind(5);
    }
  }

  void drawScreenQuad() {
    frame_buffer_.unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    screen_shader_->bind();
    frame_buffer_.getTexture(2)->bind(0);
    screen_quad_->bind();
    glDrawElements(GL_TRIANGLES, screen_quad_->count(), GL_UNSIGNED_INT, nullptr);
  }

  void createDummyTextures(World& world) {
    DummyTextures dummies{};

    TextureViewDescriptor view{};
    view.width = 1;
    view.height = 1;
    view.internal_format = GL_RGB;
    view.image_format = GL_RGB;
    view.type = GL_UNSIGNED_BYTE;

    unsigned char data[3] = {255, 255, 255};

    TextureHandle white = TextureBuilder()
        .withView(view)
        .withData(data)
        .build();
    dummies.white = white;

    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    TextureHandle black = TextureBuilder()
        .withView(view)
        .withData(data)
        .build();
    dummies.black = black;

    data[0] = 128;
    data[1] = 128;
    data[2] = 255;
    TextureHandle normal = TextureBuilder()
        .withView(view)
        .withData(data)
        .build();
    dummies.normal = normal;

    view.image_format = GL_RED;
    view.internal_format = GL_RED;
    unsigned char red_data[1] = {255};
    TextureHandle red = TextureBuilder()
        .withView(view)
        .withData(red_data)
        .build();
    dummies.red = red;

    world.addResource<DummyTextures>(dummies);
  }
};

}  // namespace sigma
