#pragma once

#include "system.h"
#include "world.h"

#include <memory>
#include <utility>
#include <vector>

namespace sigma {

class Dispatcher {
 public:
  Dispatcher() = default;
  void Start(std::shared_ptr<World> world) {
    for (auto &s : systems_) {
      s->Start(world);
    }
  }
  void Update(std::shared_ptr<World> world) {
    for (auto &s : systems_) {
      s->Update(world);
    }
  }
  void AddSystem(std::shared_ptr<System> &system) {
    systems_.push_back(std::move(system));
  }

 private:
  std::vector<std::shared_ptr<System>> systems_;
};

class GameData {
 public:
  GameData() = default;
  explicit GameData(Dispatcher dispatcher) : dispatcher_(dispatcher) {
  }
  void Start(std::shared_ptr<World> world) {
    dispatcher_.Start(world);
  }
  void Update(std::shared_ptr<World> world) {
    dispatcher_.Update(world);
  }

 protected:
  Dispatcher dispatcher_;
};

// @TODO
class GameDataBuilder {
 public:
  std::shared_ptr<GameData> Build() {
    return std::make_shared<GameData>(std::move(dispatcher_));
  }

  GameDataBuilder &WithSystem(std::shared_ptr<System> system) {
    dispatcher_.AddSystem(system);

    return *this;
  }

 private:
  Dispatcher dispatcher_;
};

}  // namespace sigma
