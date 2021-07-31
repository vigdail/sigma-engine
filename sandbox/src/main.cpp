#include <core/application.h>
#include <render/shader.h>
#include <input/input.h>
#include <window/window.h>
#include <core/components.h>
#include <render/renderer.h>
#include <components/point_light.h>
#include <core/time_system.h>

struct Moving {
  bool data;
};

class CursorHideSystem : public sigma::System {
 public:
  void update(sigma::World& world) override {
    const auto& events = world.resource<sigma::EventBus<sigma::Event>>().events;
    for (const auto& event: events) {
      std::visit(sigma::overloaded{
          [&](sigma::input_event::InputEvent ev) {
            std::visit(sigma::overloaded{
                           [&](sigma::input_event::KeyPressed event) {
                             if (event.key == sigma::KeyCode::SPACE) {
                               auto& window = world.resource<sigma::Window>();
                               window.toggleCursor();
                             }
                           },
                           [](auto) {},
                       },
                       ev);
          },
          [](auto) {},
      }, event);
    }
  }
};

class MoveSystem : public sigma::System {
 public:
  void update(sigma::World& world) override {
    auto time = world.resource<sigma::Time>();
    auto current_time = time.time;
    auto dt = time.delta_time;
    world.raw().view<sigma::Transform, Moving>().each([&](auto& transform, auto& m) {
      transform.translation.x = 2.5f * sin(current_time);
      transform.rotation.x += 1.0f * dt;
      transform.rotation.y += 0.5f * dt;
    });
  }
};

class CameraSystem : public sigma::System {
 public:
  void update(sigma::World& world) override {
    handleMouse(world);
    handleKeyboard(world);

    float dt = world.resource<sigma::Time>().delta_time;

    glm::vec3 disp{0.0f};
    if (glm::length(dir_) > glm::epsilon<float>()) {
      disp = glm::normalize(dir_) * speed_;
    }

    world.raw().view<sigma::Camera>().each([&](auto& camera) {
      camera.addYaw(dx_ * sensitivity_ * dt);
      camera.addPitch(-dy_ * sensitivity_ * dt);

      const auto& last_pos = camera.getPosition();
      camera.translate(disp * dt);
    });
  }

 private:
  bool is_first_{true};
  float last_x_{0.0f};
  float last_y_{0.0f};
  float sensitivity_{5.0f};
  float speed_{15.0f};
  float dx_{0.0f};
  float dy_{0.0f};
  glm::vec3 dir_;

 private:
  void handleMouse(sigma::World& world) {
    dx_ = 0.0f;
    dy_ = 0.0f;

    const auto& bus = world.resource<sigma::EventBus<sigma::Event>>();

    for (auto& e: bus.events) {
      std::visit(sigma::overloaded{
          [&](sigma::input_event::InputEvent ev) {
            std::visit(sigma::overloaded{
                           [&](sigma::input_event::MouseMoved event) { onMouseMoved(world, event); },
                           [](auto) {},
                       },
                       ev);
          },
          [](auto) {},
      }, e);
    }
  }

  void handleKeyboard(sigma::World& world) {
    auto input = world.resource<sigma::Input>();

    glm::vec3 dir{0.0f};
    if (input.getKeyState(sigma::KeyCode::W) == sigma::KeyState::PRESSED) {
      dir.z += 1.0f;
    }
    if (input.getKeyState(sigma::KeyCode::S) == sigma::KeyState::PRESSED) {
      dir.z -= 1.0f;
    }
    if (input.getKeyState(sigma::KeyCode::A) == sigma::KeyState::PRESSED) {
      dir.x -= 1.0f;
    }
    if (input.getKeyState(sigma::KeyCode::D) == sigma::KeyState::PRESSED) {
      dir.x += 1.0f;
    }

    dir_ = dir;
  }

  void onMouseMoved(sigma::World& world, const sigma::input_event::MouseMoved& event) {
    if (is_first_) {
      is_first_ = false;
      last_x_ = event.x;
      last_y_ = event.y;
    }
    dx_ = event.x - last_x_;
    dy_ = event.y - last_y_;

    last_x_ = event.x;
    last_y_ = event.y;
  }
};

class GameState : public sigma::SimpleState {
 public:

  void onStart(sigma::StateData state_data) override {
    std::cout << "[LOG] Game start" << std::endl;

    auto[world, data] = state_data;

    auto camera = world.createEntity();
    auto camera_component = sigma::Camera::perspective(45.0f, 960.0f / 540.0f, 0.01f, 100.0f);
    camera_component.setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    camera.addComponent<sigma::Camera>(camera_component);

    auto m = sigma::MeshFactory::createCube();
    sigma::MeshComponent mesh{m};

    auto texture = sigma::TextureBuilder()
        .load("../assets/textures/cross.png")
        .build();

    auto cube = world.createEntity();
    cube.addComponent<sigma::Transform>(glm::vec3(0.0f, 0.0f, 2.0f));
    cube.addComponent<sigma::MeshComponent>(mesh);
    cube.addComponent<sigma::PbrMaterial>(texture);
    cube.addComponent<Moving>();

    sigma::Transform light_transform(glm::vec3(-2.0f, 0.0f, 3.0f));
    light_transform.scale = glm::vec3(0.3f);
    auto light = world.createEntity();
    light.addComponent<sigma::Transform>(light_transform);
    light.addComponent<sigma::MeshComponent>(mesh);
    light.addComponent<sigma::PointLight>(glm::vec3(0.5f));

    light_transform.translation = glm::vec3(0.0f, 2.0f, 5.0f);
    auto light_2 = world.createEntity();
    light_2.addComponent<sigma::Transform>(light_transform);
    light_2.addComponent<sigma::MeshComponent>(mesh);
    light_2.addComponent<sigma::PointLight>(glm::vec3(1.0f, 0.0f, 0.0f));

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
      .withSystem(std::make_shared<sigma::TimeSystem>())
      .withSystem(std::make_shared<sigma::InputSystem>())
      .withSystem(std::make_shared<sigma::RenderSystem>())
      .withSystem(std::make_shared<MoveSystem>())
      .withSystem(std::make_shared<CursorHideSystem>())
      .withSystem(std::make_shared<CameraSystem>());
  sigma::Application app(std::make_shared<LoadingState>(), data);
  app.run();
}
