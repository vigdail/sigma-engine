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
  void start(World& world) {
    for (auto& s : systems_) {
      s->start(world);
    }
  }
  void update(World& world) {
    for (auto& s : systems_) {
      s->update(world);
    }
  }
  void addSystem(const std::shared_ptr<System>& system) {
    systems_.push_back(system);
  }

 private:
  std::vector<std::shared_ptr<System>> systems_;
};

class GameData {
 public:
  GameData() = default;
  explicit GameData(Dispatcher dispatcher) : dispatcher_(dispatcher) {
  }
  void start(World& world) {
    dispatcher_.start(world);
  }
  void update(World& world) {
    dispatcher_.update(world);
  }

 protected:
  Dispatcher dispatcher_;
};

class GameDataBuilder {
 public:
  GameData build() {
    return GameData(std::move(dispatcher_));
  }

  GameDataBuilder& withSystem(std::shared_ptr<System> system) {
    dispatcher_.addSystem(system);

    return *this;
  }

 private:
  Dispatcher dispatcher_;
};

}  // namespace sigma
