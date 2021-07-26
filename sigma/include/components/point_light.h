#pragma once

namespace sigma {

struct PointLight {
  glm::vec3 color{1.0f};
  float intensity{1.0f};
  float radius{1.0f};
};

}
