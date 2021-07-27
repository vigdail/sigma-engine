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
    camera.updateVectors();

    return camera;
  }

  static Camera ortho(float width, float height, float near, float far) {
    Camera camera{};
    camera.projection = glm::ortho(0.0f, width, height, 0.0f, near, far);
    camera.updateVectors();

    return camera;
  }

  void setPosition(glm::vec3 pos) {
    position = pos;
    recalculateView();
  }

  void translate(const glm::vec3& vec) {
    position += right * vec.x;
    position += front * vec.z;
    recalculateView();
  }

  const glm::vec3& getPosition() const {
    return position;
  }

  void addPitch(float delta) {
    pitch += delta;
    if (pitch > 89.0f) {
      pitch = 89.0f;
    }
    if (pitch < -89.0f) {
      pitch = -89.0f;
    }
    updateVectors();
  }

  void addYaw(float delta) {
    yaw += delta;
    updateVectors();
  }

 private:
  glm::vec3 position{glm::vec3(0.0f)};
  glm::vec3 up{0.0f, 1.0f, 0.0f};
  glm::vec3 front{0.0f, 0.0f, -1.0f};
  glm::vec3 right{1.0f, 0.0f, 0.0};
  float yaw{-90.0f};
  float pitch{0.0f};

 private:
  void recalculateView() {
    view = glm::lookAt(position, position + front, up);
  }

  void updateVectors() {
    glm::vec3 f;
    f.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    f.y = sin(glm::radians(pitch));
    f.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    front = glm::normalize(f);
    right = glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f));
    up = glm::cross(right, front);

    recalculateView();
  }
};
}

