#pragma once

#include "mesh.h"

namespace sigma {

class MeshFactory {
 public:
  static MeshHandle createQuad();
  static MeshHandle createCube();
};

}
