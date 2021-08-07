#pragma once

#include <functional>
#include "pch.h"
#include "event/events.h"
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

  template<class T>
  void addEvent() {
    addResource<Events<T>>();

    event_update_callbacks_.push_back([this]() {
      auto& events = resource<Events<T>>();
      events.update();
    });
  }

  template<typename T>
  Events<T>& eventBus() {
    return resource<Events<T>>();
  }

  void updateEvents() {
    for (const auto& cb: event_update_callbacks_) {
      cb();
    }
  }

  entt::registry& raw() {
    return registry_;
  }

 private:
  entt::registry registry_;
  std::vector<std::function<void()>> event_update_callbacks_;
};

}  // namespace sigma
