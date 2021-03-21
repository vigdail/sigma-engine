#pragma once

#include <memory>

namespace sigma {

class System {
 public:
  virtual ~System() = default;
  virtual void Update(std::shared_ptr<World> world) = 0;
};

}  // namespace sigma
