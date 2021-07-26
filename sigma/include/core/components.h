#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sigma {
struct Transform {
  glm::vec3 translation{};
  glm::vec3 rotation{};
  glm::vec3 scale{1.0f};

  Transform() = default;
  Transform(const Transform&) = default;

  Transform(const glm::vec3& translation) : translation{translation} {}

  glm::mat4 transform() const {
    glm::mat4 rotation_matrix = glm::toMat4(glm::quat(rotation));

    return glm::translate(glm::mat4(1.0f), translation)
        * rotation_matrix
        * glm::scale(glm::mat4(1.0f), scale);
  }
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

  const glm::vec3& getPosition() const {
    return position;
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

