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
#include "util/util.h"

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
    world.addEvent<window_event::Resized>();
    world.addEvent<window_event::Moved>();
    world.addEvent<window_event::CloseRequested>();
    world.addEvent<window_event::Destroyed>();
    world.addEvent<window_event::Focused>();
    world.addEvent<input_event::KeyPressed>();
    world.addEvent<input_event::KeyReleased>();
    world.addEvent<input_event::MouseMoved>();
    world.addEvent<input_event::MouseButtonPressed>();
    world.addEvent<input_event::MouseButtonReleased>();
    world.addEvent<input_event::MouseWheelMoved>();
  }

  void update(World& world) override {
    auto& window = world.resource<Window>();
    window.swapBuffers();

    auto window_events = window.pollEvents();
    for (auto event: window_events) {
      std::visit([&](const auto& ev) {
        auto& bus = getBus(world, ev);
        bus.send(ev);
      }, event);
    }
  }

 private:
  WindowConfig config_;

 private:
  template<typename T>
  static Events<T>& getBus(World& world, T) {
    return world.eventBus<T>();
  }
};

}  // namespace sigma
