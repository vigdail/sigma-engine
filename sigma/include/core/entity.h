#pragma once

#include <utility>

#include "pch.h"
#include "world.h"

namespace sigma {

class Entity final {
 public:
  Entity() = default;
  explicit Entity(World* world) : world_(world) {
    handle_ = world->registry_.create();
  }
  template<typename T, typename... Args>
  Entity& addComponent(Args&&... args) {
    world_->registry_.emplace<T>(handle_, std::forward<Args>(args)...);

    return *this;
  }

 private:
  World* world_;
  entt::entity handle_;
};

}  // namespace sigma
