#pragma once

#include "pch.h"

namespace sigma {

class Entity;

class World {
  friend Entity;

 public:
  World() = default;
  ~World() = default;
  Entity CreateEntity();
  entt::registry &Raw() {
    return registry_;
  }

 private:
  entt::registry registry_;
};

}  // namespace sigma
