#pragma once

#include "pch.h"
#include <utility>

namespace sigma {

class Entity;

class World {
  friend Entity;

 public:
  World() = default;
  ~World() = default;
  Entity CreateEntity();
  template <typename T>
  T &Resource() {
    return registry_.ctx<T>();
  }
  template <typename T, typename... Args>
  T &AddResource(Args &&... resource) {
    return registry_.set<T>(std::forward<Args>(resource)...);
  }

  entt::registry &Raw() {
    return registry_;
  }

 private:
  entt::registry registry_;
};

}  // namespace sigma
