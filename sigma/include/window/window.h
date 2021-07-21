#pragma once

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include "pch.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core/system.h"
#include "core/world.h"
#include "event/event.h"

namespace sigma {

class Window {
 public:
  Window(int width, int height) noexcept;
  ~Window();
  std::vector<Event> pollEvents();
  void swapBuffers() const;
  int getWidth() const {
    return width_;
  }
  int getHeight() const {
    return height_;
  }
  void setTitle(std::string title) const;
  bool shouldClose() const {
    return glfwWindowShouldClose(window_);
  }
  void hideCursor();
  void showCursor();
  void toggleCursor();
  bool isCursorHidden() const;

 private:
  GLFWwindow* window_;
  int width_;
  int height_;
  std::vector<Event> events_;

 private:
  void initOpenGl();
  void createWindow();
  void setupCallbacks();
  void moveToCenter();
};

struct WindowConfig {
  int width = 800;
  int height = 600;
};

class WindowSystem : public System {
 public:
  explicit WindowSystem(WindowConfig config) : config_(config) {
  }
  void start(World& world) override {
    world.addResource<Window>(config_.width, config_.height);
  }

  void update(World& world) override {
    auto& window = world.resource<Window>();
    window.swapBuffers();

    world.addResource<EventBus<Event>>(std::move(window.pollEvents()));
  }

 private:
  WindowConfig config_;
};

}  // namespace sigma
