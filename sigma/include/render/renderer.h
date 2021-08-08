
#pragma once

#include <components/point_light.h>
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
  TextureHandle albedo;
  TextureHandle normalmap;
  TextureHandle metallic;
  TextureHandle roughness;
  TextureHandle ao;
  TextureHandle emission;
};

struct DummyTextures {
  Ref<Texture> white;
  Ref<Texture> black;
  Ref<Texture> red;
  Ref<Texture> normal;
};

class RenderSystem : public System {

 public:
  void start(World& world) override {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
    descriptor.depth_format = GL_DEPTH24_STENCIL8;
    frame_buffer_ = FrameBuffer(descriptor);

    g_buffer_shader_ = ShaderBuilder()
        .loadModule("../assets/shaders/g_buffer.vert", sigma::ShaderStage::VERTEX)
        .loadModule("../assets/shaders/g_buffer.frag", sigma::ShaderStage::FRAGMENT)
        .build();

    g_buffer_shader_->setInt("albedo", 0);
    g_buffer_shader_->setInt("normalmap", 1);
    g_buffer_shader_->setInt("metallic", 2);
    g_buffer_shader_->setInt("roughness", 3);
    g_buffer_shader_->setInt("ao", 4);
    g_buffer_shader_->setInt("emissionmap", 5);

    screen_quad_ = MeshFactory::createQuad();
    screen_shader_ = ShaderBuilder()
        .loadModule("../assets/shaders/screen.vert", ShaderStage::VERTEX)
        .loadModule("../assets/shaders/screen.frag", ShaderStage::FRAGMENT)
        .build();

    pointlight_shader_ = ShaderBuilder()
        .loadModule("../assets/shaders/point_light.vert", ShaderStage::VERTEX)
        .loadModule("../assets/shaders/point_light.frag", ShaderStage::FRAGMENT)
        .build();

    light_shader_ = ShaderBuilder()
        .loadModule("../assets/shaders/light.vert", ShaderStage::VERTEX)
        .loadModule("../assets/shaders/light.frag", ShaderStage::FRAGMENT)
        .build();
  }

  void update(World& world) override {
    frame_buffer_.bind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto cameras = world.raw().view<Camera>();
    if (cameras.empty()) {
      return;
    }

    const auto& camera = world.raw().get<Camera>(cameras[0]);

    auto view = world.raw().view<MeshComponent, Transform, PbrMaterial>();
    view.each([&](const auto& mesh, const auto& transform, const auto& material) {
      auto& shader = g_buffer_shader_;
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

    frame_buffer_.unbind();
    drawScreenQuad(world, camera);
  }

 private:
  FrameBuffer frame_buffer_;
  MeshHandle screen_quad_;
  ShaderHandle g_buffer_shader_;
  ShaderHandle screen_shader_;
  ShaderHandle pointlight_shader_;
  ShaderHandle light_shader_;

 private:
  void bindTextures(const PbrMaterial& material, const World& world) {
    const auto& dummies = world.resource<DummyTextures>();
    const auto& textures = world.resource<ResourceManager<Texture>>();
    if (material.albedo) {
      textures.get(material.albedo)->bind(0);
    } else {
      dummies.white->bind(0);
    }
    if (material.normalmap) {
      textures.get(material.normalmap)->bind(1);
    } else {
      dummies.normal->bind(1);
    }
    if (material.metallic) {
      textures.get(material.metallic)->bind(2);
    } else {
      dummies.red->bind(2);
    }
    if (material.roughness) {
      textures.get(material.roughness)->bind(3);
    } else {
      dummies.red->bind(3);
    }
    if (material.ao) {
      textures.get(material.ao)->bind(4);
    } else {
      dummies.red->bind(4);
    }
    if (material.emission) {
      textures.get(material.emission)->bind(5);
    } else {
      dummies.black->bind(5);
    }
  }

  void drawScreenQuad(World& world, const Camera& camera) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto width = frame_buffer_.getWidth();
    auto height = frame_buffer_.getHeight();
    frame_buffer_.bindRead();
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, width, height,
                      0, 0, width, height,
                      GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    pointlight_shader_->bind();
    pointlight_shader_->setVec3("camera_position", camera.getPosition());
    pointlight_shader_->setInt("position_metal", 0);
    pointlight_shader_->setInt("normal_roughness", 1);
    pointlight_shader_->setInt("albedo_ao", 2);
    pointlight_shader_->setInt("emission", 3);
    frame_buffer_.getTexture(0)->bind(0);
    frame_buffer_.getTexture(1)->bind(1);
    frame_buffer_.getTexture(2)->bind(2);
    frame_buffer_.getTexture(3)->bind(3);

    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    glDepthMask(GL_FALSE);

    auto view = world.raw().view<PointLight, Transform, MeshComponent>();
    view.each([&](const auto& light, const auto& transform, const auto&) {
      pointlight_shader_->setVec3("light_position", transform.translation);
      pointlight_shader_->setVec3("light_color", light.color);
      pointlight_shader_->setFloat("light_radius", light.radius);
      pointlight_shader_->setFloat("light_intensity", light.intensity);

      screen_quad_->bind();
      glDrawElements(GL_TRIANGLES, screen_quad_->count(), GL_UNSIGNED_INT, nullptr);
    });

    light_shader_->bind();
    light_shader_->setMat4("view", camera.view);
    light_shader_->setMat4("projection", camera.projection);

    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);

    view.each([&](const auto& light, const auto& transform, const auto& mesh_component) {
      light_shader_->setMat4("model", transform.transform());
      light_shader_->setVec3("color", light.color);
      const auto& mesh = mesh_component.mesh;

      mesh->bind();
      glDrawElements(GL_TRIANGLES, mesh->count(), GL_UNSIGNED_INT, nullptr);
    });

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

    auto white = TextureBuilder()
        .withView(view)
        .withData(data)
        .build();
    dummies.white = white;

    data[0] = 0;
    data[1] = 0;
    data[2] = 0;
    auto black = TextureBuilder()
        .withView(view)
        .withData(data)
        .build();
    dummies.black = black;

    data[0] = 128;
    data[1] = 128;
    data[2] = 255;
    auto normal = TextureBuilder()
        .withView(view)
        .withData(data)
        .build();
    dummies.normal = normal;

    view.image_format = GL_RED;
    view.internal_format = GL_RED;
    unsigned char red_data[1] = {255};
    auto red = TextureBuilder()
        .withView(view)
        .withData(red_data)
        .build();
    dummies.red = red;

    world.addResource<DummyTextures>(dummies);
  }
};

}  // namespace sigma
