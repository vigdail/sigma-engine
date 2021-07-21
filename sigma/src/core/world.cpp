
#include "core/world.h"
#include "core/entity.h"

namespace sigma {

Entity World::createEntity() {
  return Entity(this);
}

}  // namespace sigma
