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
  void Start(World &world) {
    for (auto &s : systems_) {
      s->Start(world);
    }
  }
  void Update(World &world) {
    for (auto &s : systems_) {
      s->Update(world);
    }
  }
  void AddSystem(const std::shared_ptr<System> &system) {
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
  void Start(World &world) {
    dispatcher_.Start(world);
  }
  void Update(World &world) {
    dispatcher_.Update(world);
  }

 protected:
  Dispatcher dispatcher_;
};

class GameDataBuilder {
 public:
  GameData Build() {
    return GameData(std::move(dispatcher_));
  }

  GameDataBuilder &WithSystem(std::shared_ptr<System> system) {
    dispatcher_.AddSystem(system);

    return *this;
  }

 private:
  Dispatcher dispatcher_;
};

}  // namespace sigma
