#include <core/application.h>
#include <input/input.h>
#include <window/window.h>

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

struct GameState : public sigma::SimpleState {
 public:
  void OnStart(sigma::StateData stateData) override {
    auto [world, data] = stateData;
    auto orc = world->CreateEntity();
    orc.AddComponent<Name>("Orc");
    orc.AddComponent<Health>(10, 10);
    orc.AddComponent<Transform>(10, 10, 100, 100);

    auto goblin = world->CreateEntity();
    goblin.AddComponent<Name>("Goblin");
    goblin.AddComponent<Health>(5, 5);
    goblin.AddComponent<Transform>(150, 10, 100, 100);
  }
};

bool PointInRect(int x, int y, Transform rect) {
  return ((x >= rect.x && x <= rect.x + rect.width) && (y >= rect.y && y <= rect.y + rect.height));
}

class MySystem : public sigma::System {
 public:
  void Update(std::shared_ptr<sigma::World> world) override {
    auto &bus = world->Raw().ctx<sigma::EventBus<sigma::Event>>();

    for (auto &e : bus.events) {
      std::visit(sigma::overloaded{
                   [&](sigma::InputEvent::InputEvent ev) {
                     std::visit(sigma::overloaded{
                                  [&](sigma::InputEvent::MouseButtonPressed event) { OnMousePressed(world); },
                                  [](auto) {},
                                },
                                ev);
                   },
                   [](auto) {},
                 },
                 e);
    }
  }

  void OnMousePressed(std::shared_ptr<sigma::World> world) {
    auto &input = world->Raw().ctx<sigma::Input>();
    auto mousePos = input.GetMousePosition();
    std::cout << "[LOG] Mouse clicked: " << mousePos.x << ", " << mousePos.y << std::endl;
    world->Raw().view<Name, Transform, Health>().each([&](auto entity, auto &name, auto &transform, auto &health) {
      if (!PointInRect(mousePos.x, mousePos.y, transform)) {
        return;
      }
      health.value--;
      std::cout << "[LOG] " << name.name << " health: " << health.value << "/" << health.max_value << std::endl;
      if (health.value == 0) {
        world->Raw().destroy(entity);
        std::cout << "[LOG] " << name.name << " killed" << std::endl;
      }
    });
  }
};

int main() {
  auto data = sigma::GameDataBuilder()
                .WithSystem(std::make_shared<sigma::WindowSystem>(sigma::WindowConfig{ 960, 540 }))
                .WithSystem(std::make_shared<sigma::InputSystem>())
                .WithSystem(std::make_shared<MySystem>());
  sigma::Application app(std::make_shared<GameState>(), data);
  app.Run();
}
