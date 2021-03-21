#include <core/application.h>
#include <window/window.h>

struct Health {
  int value;
  int max_value;
};

struct GameState : public sigma::SimpleState {
 public:
  void OnStart(sigma::StateData stateData) override {
    auto [world, data] = stateData;
    player_ = world->CreateEntity();
    player_.AddComponent<Health>(10, 10);
  }

 protected:
  sigma::Entity player_;
};

class MySystem : public sigma::System {
 public:
  void Update(std::shared_ptr<sigma::World> world) override {
    auto &bus = world->Raw().ctx<sigma::EventBus<sigma::Event>>();

    for (auto &e : bus.events) {
      std::visit(sigma::overloaded{
                   [&](sigma::InputEvent::InputEvent ev) {
                     std::visit(sigma::overloaded{
                                  [](sigma::InputEvent::KeyPressed event) {
                                    std::cout << "[LOG] Key pressed: " << static_cast<int>(event.key) << std::endl;
                                  },
                                  [&](sigma::InputEvent::MouseButtonPressed event) {
                                    auto view = world->Raw().view<Health>();
                                    for (auto entity : view) {
                                      auto &health = world->Raw().get<Health>(entity);
                                      health.value--;
                                      std::cout << "[LOG] Player health: " << health.value << "/" << health.max_value
                                                << std::endl;
                                      if (health.value == 0) {
                                        world->Raw().destroy(entity);
                                        std::cout << "[LOG] Player killed" << std::endl;
                                      }
                                    }
                                  },
                                  [](auto) {},
                                },
                                ev);
                   },
                   [](auto) {},
                 },
                 e);
    }
  }
};

int main() {
  auto data = sigma::GameDataBuilder()
                .WithSystem(std::make_shared<sigma::WindowSystem>(sigma::WindowConfig{ 960, 540 }))
                .WithSystem(std::make_shared<MySystem>());
  sigma::Application app(std::make_shared<GameState>(), data);
  app.Run();
}
