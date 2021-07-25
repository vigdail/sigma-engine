#include <core/application.h>
#include <render/shader.h>
#include <input/input.h>
#include <window/window.h>
#include <core/components.h>
#include <render/renderer.h>

class MoveSystem : public sigma::System {
 public:
  void update(sigma::World& world) override {
    auto time = glfwGetTime();
    auto dt = time - last_time_;
    last_time_ = time;
    world.raw().view<sigma::Transform>().each([&](auto& transform) {
      transform.translation.x = 2.5f * sin(time);
      transform.rotation.x += 1.0f * dt;
      transform.rotation.y += 0.5f * dt;
    });
  }

 private:
  double last_time_ = 0.0f;
};

class GameState : public sigma::SimpleState {
 public:

  void onStart(sigma::StateData state_data) override {
    std::cout << "[LOG] Game start" << std::endl;

    auto[world, data] = state_data;

    auto camera = world.createEntity();
    auto camera_component = sigma::Camera::perspective(45.0f, 960.0f / 540.0f, 0.01f, 100.0f);
    camera_component.setPosition(glm::vec3(0.0f, 0.0f, 3.0f));
    camera.addComponent<sigma::Camera>(camera_component);

    auto m = sigma::MeshFactory::createCube();
    sigma::MeshComponent mesh{m};

    auto shader = sigma::ShaderBuilder()
        .loadModule("../assets/shaders/g_buffer.vert", sigma::ShaderStage::VERTEX)
        .loadModule("../assets/shaders/g_buffer.frag", sigma::ShaderStage::FRAGMENT)
        .build();

    auto texture = sigma::TextureBuilder()
        .load("../assets/textures/cross.png")
        .build();

    auto cube = world.createEntity();
    cube.addComponent<sigma::Transform>(glm::vec3(0.0f, 0.0f, 0.0f));
    cube.addComponent<sigma::MeshComponent>(mesh);
    cube.addComponent<sigma::PbrMaterial>(shader, texture);

    dispatcher_.start(state_data.world);
  }

  sigma::Transition update(sigma::StateData state_data) override {
    dispatcher_.update(state_data.world);
    return sigma::transition::None();
  }

 private:
  sigma::Dispatcher dispatcher_;
};

struct LoadingState : public sigma::SimpleState {
 public:
  void onStart(sigma::StateData state_data) override {
    std::cout << "[LOG] Loading start" << std::endl;
  }

  sigma::Transition update(sigma::StateData state_data) override {
    return sigma::transition::Switch{std::make_shared<GameState>()};
  }
};

int main() {
  auto data = sigma::GameDataBuilder()
      .withSystem(std::make_shared<sigma::WindowSystem>(sigma::WindowConfig{960, 540}))
      .withSystem(std::make_shared<sigma::InputSystem>())
      .withSystem(std::make_shared<sigma::RenderSystem>())
      .withSystem(std::make_shared<MoveSystem>());
  sigma::Application app(std::make_shared<LoadingState>(), data);
  app.run();
}
