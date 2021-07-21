#pragma once

#include <memory>

#include "world.h"

namespace sigma {

class System {
 public:
  virtual ~System() = default;
  virtual void start(World& world) {
  }
  virtual void update(World& world) = 0;
};

}  // namespace sigma
