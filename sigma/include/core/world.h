#pragma once

#include <functional>
#include "pch.h"
#include <event/event.h>
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
    addResource<EventBus<T>>();
    event_clear_callbacks_.push_back([this]() {
      auto& events = resource<EventBus<T>>().events;
      events.clear();
    });
  }

  template<typename T>
  EventBus<T>& eventBus() {
    return resource<EventBus<T>>();
  }

  void clearEvents() {
    for (const auto& cb: event_clear_callbacks_) {
      cb();
    }
  }

  entt::registry& raw() {
    return registry_;
  }

 private:
  entt::registry registry_;
  std::vector<std::function<void()>> event_clear_callbacks_;
};

}  // namespace sigma
