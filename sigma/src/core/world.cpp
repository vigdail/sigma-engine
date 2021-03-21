
#include "core/world.h"
#include "core/entity.h"

namespace sigma {

Entity World::CreateEntity() {
  return Entity(this);
}

}  // namespace sigma
