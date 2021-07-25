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
  Entity createEntity();

  template<typename T>
  T& resource() {
    return registry_.ctx<T>();
  }

  template<class T>
  const T& resource() const {
    return registry_.ctx<T>();
  }

  template<typename T, typename... Args>
  T& addResource(Args&& ... resource) {
    return registry_.set<T>(std::forward<Args>(resource)...);
  }

  entt::registry& raw() {
    return registry_;
  }

 private:
  entt::registry registry_;
};

}  // namespace sigma
