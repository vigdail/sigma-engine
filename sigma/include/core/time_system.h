#pragma once

#include <GLFW/glfw3.h>
#include "core/system.h"

namespace sigma {

struct Time {
  float time;
  float delta_time;
};

class TimeSystem : public sigma::System {
 public:
  void update(World& world) override {
    auto time = glfwGetTime();
    auto dt = time - last_time_;
    last_time_ = time;
    world.addResource<Time>(time, dt);
  }

 private:
  double last_time_ = 0.0f;
};

}
