#include <core/application.h>
#include <render/shader.h>
#include <input/input.h>
#include <window/window.h>
#include <core/components.h>
#include <render/renderer.h>
#include <components/point_light.h>
#include <core/time_system.h>

#include <resources/resource_manager.h>

struct Moving {
  bool data;
};

class CursorHideSystem : public sigma::System {
 public:
  void update(sigma::World& world) override {
    auto& input = world.resource<sigma::Input>();
    if (input.getKeyState(sigma::KeyCode::SPACE) == sigma::KeyState::PRESSED) {
      auto& window = world.resource<sigma::Window>();
      window.toggleCursor();
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
    auto input = world.resource<sigma::Input>();
    auto move = input.getMouseMove();
    dx_ = move.x;
    dy_ = move.y;
  }

  void handleKeyboard(sigma::World& world) {
    auto input = world.resource<sigma::Input>();

    glm::vec3 dir{0.0f};
    if (input.isPressedOrDown(sigma::KeyCode::W)) {
      dir.z += 1.0f;
    }
    if (input.isPressedOrDown(sigma::KeyCode::S)) {
      dir.z -= 1.0f;
    }
    if (input.isPressedOrDown(sigma::KeyCode::A)) {
      dir.x -= 1.0f;
    }
    if (input.isPressedOrDown(sigma::KeyCode::D)) {
      dir.x += 1.0f;
    }

    dir_ = dir;
  }
};

class GameState : public sigma::State {
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

    auto& rm = state_data.world.resource<sigma::ResourceManager<sigma::Texture>>();
    auto albedo = rm.request("../assets/textures/cross.png");

    auto cube = world.createEntity();
    cube.addComponent<sigma::Transform>(glm::vec3(0.0f, 0.0f, 2.0f));
    cube.addComponent<sigma::MeshComponent>(mesh);
    cube.addComponent<sigma::PbrMaterial>(albedo);
    cube.addComponent<Moving>();

    sigma::Transform tr;
    tr.translation = glm::vec3(0.0f, -2.0f, 0.0f);
    tr.scale = glm::vec3(100.0f, 0.1f, 100.0f);
    auto floor = world.createEntity();
    floor.addComponent<sigma::Transform>(tr);
    floor.addComponent<sigma::MeshComponent>(mesh);
    floor.addComponent<sigma::PbrMaterial>();

    tr.translation = glm::vec3(0.0f, -2.0f, 0.0f);
    tr.rotation = glm::vec3(glm::radians(90.0f), 0.0f, 0.0f);
    tr.scale = glm::vec3(100.0f, 0.1f, 100.0f);
    auto wall = world.createEntity();
    wall.addComponent<sigma::Transform>(tr);
    wall.addComponent<sigma::MeshComponent>(mesh);
    wall.addComponent<sigma::PbrMaterial>();

    sigma::Transform light_transform(glm::vec3(-2.0f, 0.0f, 4.0f));
    light_transform.scale = glm::vec3(0.3f);
    auto light = world.createEntity();
    light.addComponent<sigma::Transform>(light_transform);
    light.addComponent<sigma::MeshComponent>(mesh);
    light.addComponent<sigma::PointLight>(glm::vec3(0.0f, 0.0f, 1.0f), 5.0f, 10.0f);

    light_transform.translation = glm::vec3(2.0f, 0.0f, 4.0f);
    auto light_2 = world.createEntity();
    light_2.addComponent<sigma::Transform>(light_transform);
    light_2.addComponent<sigma::MeshComponent>(mesh);
    light_2.addComponent<sigma::PointLight>(glm::vec3(1.0f, 0.0f, 0.0f), 5.0f, 10.0f);
  }

};

struct LoadingState : public sigma::State {
 public:
  void onStart(sigma::StateData state_data) override {
    sigma::State::onStart(state_data);
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
      .withSystem(std::make_shared<sigma::TimeSystem>())
      .withSystem(std::make_shared<sigma::RenderSystem>())
      .withSystem(std::make_shared<MoveSystem>())
      .withSystem(std::make_shared<CursorHideSystem>())
      .withSystem(std::make_shared<CameraSystem>());
  sigma::Application app(std::make_shared<LoadingState>(), data);
  app.run();
}
