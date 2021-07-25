#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sigma {
struct Transform {
  int x;
  int y;
  int width;
  int height;
};

struct Camera {
  glm::mat4 view{};
  glm::mat4 projection{};

  static Camera perspective(float fov, float aspect, float near, float far) {
    Camera camera{};
    camera.projection = glm::perspective(fov, aspect, near, far);
    camera.recalculateView();

    return camera;
  }

  static Camera ortho(float width, float height, float near, float far) {
    Camera camera{};
    camera.projection = glm::ortho(0.0f, width, height, 0.0f, near, far);
    camera.recalculateView();

    return camera;
  }

  void setPosition(glm::vec3 pos) {
    position = pos;
    recalculateView();
  }

  void setLookTarget(glm::vec3 target_position) {
    target = target_position;
    recalculateView();
  }

 private:
  glm::vec3 position{glm::vec3(0.0f)};
  glm::vec3 target{glm::vec3(0.0f)};

 private:
  void recalculateView() {
    view = glm::lookAt(position, target, glm::vec3(0.0f, 1.0f, 0.0f));
  }
};
}

