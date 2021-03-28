#pragma once

#include <memory>

#include "world.h"

namespace sigma {

class System {
 public:
  virtual ~System() = default;
  virtual void Start(World &world) {
  }
  virtual void Update(World &world) = 0;
};

}  // namespace sigma
