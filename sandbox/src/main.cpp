#include <core/application.h>

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
  void Update(std::shared_ptr<sigma::World> world) override {
    std::cout << "[LOG] MySystem update: " << count++ << std::endl;
    if (count == 5) {
      auto &bus = world->Raw().ctx<sigma::EventBus<sigma::Event>>();
      bus.events.push_back(sigma::WindowEvent::CloseRequested());
    }
  }

 private:
  int count = 0;
};

int main() {
  auto data = sigma::GameDataBuilder().WithSystem(std::make_shared<MySystem>());
  sigma::Application app(std::make_shared<GameState>(), data);
  app.Run();
}
