#include <core/application.h>
#include <render/shader.h>
#include <input/input.h>
#include <window/window.h>

struct MissionCompleteEvent {};

struct Health {
  int value;
  int max_value;
};

struct Name {
  std::string name;
};

struct Transform {
  int x;
  int y;
  int width;
  int height;
};

class CompleteSystem : public sigma::System {
 public:
  void update(sigma::World& world) override {
    auto view = world.raw().view<Name, Transform, Health>();
    if (view.size() == 0) {
      auto& bus = world.resource<sigma::EventBus<MissionCompleteEvent>>();
      bus.events.push_back(MissionCompleteEvent());
    }
  }
};

bool pointInRect(int x, int y, Transform rect) {
  return ((x >= rect.x && x <= rect.x + rect.width) && (y >= rect.y && y <= rect.y + rect.height));
}

class MySystem : public sigma::System {
 public:
  void update(sigma::World& world) override {
    auto& bus = world.resource<sigma::EventBus<sigma::Event>>();

    for (auto& e : bus.events) {
      std::visit(sigma::overloaded{
                     [&](sigma::input_event::InputEvent ev) {
                       std::visit(sigma::overloaded{
                                      [&](sigma::input_event::MouseButtonPressed event) { onMousePressed(&world); },
                                      [](auto) {},
                                  },
                                  ev);
                     },
                     [](auto) {},
                 },
                 e);
    }
  }

  void onMousePressed(sigma::World* world) {
    auto& input = world->resource<sigma::Input>();
    auto mouse_pos = input.getMousePosition();
    std::cout << "[LOG] Mouse clicked: " << mouse_pos.x << ", " << mouse_pos.y << std::endl;
    world->raw().view<Name, Transform, Health>().each(
        [&](auto entity, const auto& name, const auto& transform, auto& health) {
          if (!pointInRect(mouse_pos.x, mouse_pos.y, transform)) {
            return;
          }
          health.value--;
          std::cout << "[LOG] " << name.name << " health: " << health.value << "/" << health.max_value << std::endl;
          if (health.value == 0) {
            std::cout << "[LOG] " << name.name << " killed" << std::endl;
            world->raw().destroy(entity);
          }
        });
  }
};

class GameState : public sigma::SimpleState {
 public:
  GameState() {
    auto system = std::make_shared<CompleteSystem>();
    dispatcher_.addSystem(system);
  }
  void onStart(sigma::StateData state_data) override {
    std::cout << "[LOG] Game start" << std::endl;

    auto [world, data] = state_data;
    auto orc = world.createEntity();
    orc.addComponent<Name>("Orc");
    orc.addComponent<Health>(10, 10);
    orc.addComponent<Transform>(10, 10, 100, 100);

    auto goblin = world.createEntity();
    goblin.addComponent<Name>("Goblin");
    goblin.addComponent<Health>(5, 5);
    goblin.addComponent<Transform>(150, 10, 100, 100);

    state_data.world.addResource<sigma::EventBus<MissionCompleteEvent>>();

    dispatcher_.start(state_data.world);
  }

  sigma::Transition update(sigma::StateData state_data) override {
    dispatcher_.update(state_data.world);

    if (state_data.world.resource<sigma::EventBus<MissionCompleteEvent>>().events.size() > 0) {
      return sigma::transition::Quit();
    }
    return sigma::transition::None();
  }

 private:
  sigma::Dispatcher dispatcher_;
};

struct LoadingState : public sigma::SimpleState {
 public:
  void onStart(sigma::StateData state_data) override {
    std::cout << "[LOG] Loading start" << std::endl;

    sigma::Shader shader = sigma::ShaderBuilder()
        .loadModule("../assets/shaders/solid_color.vert", sigma::ShaderStage::VERTEX)
        .loadModule("../assets/shaders/solid_color.frag", sigma::ShaderStage::FRAGMENT)
        .build();
  }
  sigma::Transition update(sigma::StateData state_data) override {
    return sigma::transition::Switch{std::make_shared<GameState>()};
  }
};

int main() {
  auto data = sigma::GameDataBuilder()
                  .withSystem(std::make_shared<sigma::WindowSystem>(sigma::WindowConfig{960, 540}))
                  .withSystem(std::make_shared<sigma::InputSystem>())
                  .withSystem(std::make_shared<MySystem>());
  sigma::Application app(std::make_shared<LoadingState>(), data);
  app.run();
}
