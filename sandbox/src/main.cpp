#include <core/application.h>
#include <input/input.h>
#include <util/util.h>
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
  void Update(sigma::World &world) override {
    auto view = world.Raw().view<Name, Transform, Health>();
    if (view.size() == 0) {
      auto &bus = world.Resource<sigma::EventBus<MissionCompleteEvent>>();
      bus.events.push_back(MissionCompleteEvent());
    }
  }
};

bool PointInRect(int x, int y, Transform rect) {
  return ((x >= rect.x && x <= rect.x + rect.width) && (y >= rect.y && y <= rect.y + rect.height));
}

class MySystem : public sigma::System {
 public:
  void Update(sigma::World &world) override {
    auto &bus = world.Resource<sigma::EventBus<sigma::Event>>();

    for (auto &e : bus.events) {
      std::visit(sigma::overloaded{
                   [&](sigma::InputEvent::InputEvent ev) {
                     std::visit(sigma::overloaded{
                                  [&](sigma::InputEvent::MouseButtonPressed event) { OnMousePressed(&world); },
                                  [](auto) {},
                                },
                                ev);
                   },
                   [](auto) {},
                 },
                 e);
    }
  }

  void OnMousePressed(sigma::World *world) {
    auto &input = world->Resource<sigma::Input>();
    auto mousePos = input.GetMousePosition();
    std::cout << "[LOG] Mouse clicked: " << mousePos.x << ", " << mousePos.y << std::endl;
    world->Raw().view<Name, Transform, Health>().each(
      [&](auto entity, const auto &name, const auto &transform, auto &health) {
        if (!PointInRect(mousePos.x, mousePos.y, transform)) {
          return;
        }
        health.value--;
        std::cout << "[LOG] " << name.name << " health: " << health.value << "/" << health.max_value << std::endl;
        if (health.value == 0) {
          world.Raw().destroy(entity);
          std::cout << "[LOG] " << name.name << " killed" << std::endl;
        }
      });
  }
};

struct GameState : public sigma::SimpleState {
 public:
  GameState() {
    auto system = std::make_shared<CompleteSystem>();
    dispatcher_.AddSystem(system);
  }
  void OnStart(sigma::StateData stateData) override {
    std::cout << "[LOG] Game start" << std::endl;

    auto [world, data] = stateData;
    auto orc = world.CreateEntity();
    orc.AddComponent<Name>("Orc");
    orc.AddComponent<Health>(10, 10);
    orc.AddComponent<Transform>(10, 10, 100, 100);

    auto goblin = world.CreateEntity();
    goblin.AddComponent<Name>("Goblin");
    goblin.AddComponent<Health>(5, 5);
    goblin.AddComponent<Transform>(150, 10, 100, 100);

    stateData.world.AddResource<sigma::EventBus<MissionCompleteEvent>>();

    dispatcher_.Start(stateData.world);
  }

  sigma::Transition Update(sigma::StateData stateData) override {
    dispatcher_.Update(stateData.world);

    if (stateData.world.Resource<sigma::EventBus<MissionCompleteEvent>>().events.size() > 0) {
      return sigma::transition::Quit();
    }
    return sigma::transition::None();
  }

 private:
  sigma::Dispatcher dispatcher_;
};

struct LoadingState : public sigma::SimpleState {
 public:
  void OnStart(sigma::StateData stateData) override {
    std::cout << "[LOG] Loading start" << std::endl;
  }
  sigma::Transition Update(sigma::StateData stateData) override {
    return sigma::transition::Switch{ std::make_shared<GameState>() };
  }
};

int main() {
  auto data = sigma::GameDataBuilder()
                .WithSystem(std::make_shared<sigma::WindowSystem>(sigma::WindowConfig{ 960, 540 }))
                .WithSystem(std::make_shared<sigma::InputSystem>())
                .WithSystem(std::make_shared<MySystem>());
  sigma::Application app(std::make_shared<LoadingState>(), data);
  app.Run();
}
