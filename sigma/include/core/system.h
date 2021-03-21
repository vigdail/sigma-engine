#pragma once

#include <memory>

#include "world.h"

namespace sigma {

class System {
 public:
  virtual ~System() = default;
  virtual void Start(std::shared_ptr<World> world) {
  }
  virtual void Update(std::shared_ptr<World> world) = 0;
};

}  // namespace sigma
