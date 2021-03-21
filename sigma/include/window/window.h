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
  std::vector<Event> PollEvents();
  void SwapBuffers() const;
  int GetWidth() const {
    return width_;
  }
  int GetHeight() const {
    return height_;
  }
  void SetTitle(std::string title) const;
  bool ShouldClose() const {
    return glfwWindowShouldClose(window_);
  }
  void HideCursor();
  void ShowCursor();
  void ToggleCursor();
  bool IsCursorHidden() const;

 private:
  GLFWwindow *window_;
  int width_;
  int height_;
  std::vector<Event> events_;

 private:
  void InitOpenGL();
  void CreateWindow();
  void SetupCallbacks();
  void MoveToCenter();
};

struct WindowConfig {
  int width = 800;
  int height = 600;
};
class WindowSystem : public System {
 public:
  explicit WindowSystem(WindowConfig config) : config_(config) {
  }
  void Start(std::shared_ptr<World> world) override {
    world->Raw().set<Window>(config_.width, config_.height);
  }

  void Update(std::shared_ptr<World> world) override {
    auto &window = world->Raw().ctx<Window>();
    window.SwapBuffers();

    world->Raw().set<EventBus<Event>>(std::move(window.PollEvents()));
  }

 private:
  WindowConfig config_;
};

}  // namespace sigma
