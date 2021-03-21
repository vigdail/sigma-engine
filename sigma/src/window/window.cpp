#include <iostream>

// #include "input.h"
#include "window/window.h"

namespace sigma {

Window::Window(int width, int height) noexcept : width_(width), height_(height) {
  InitOpenGL();
  CreateWindow();
  MoveToCenter();
  SetupCallbacks();
}

Window::~Window() {
  glfwTerminate();
}

std::vector<Event> Window::PollEvents() {
  glfwPollEvents();
  return std::move(events_);
}

void Window::SwapBuffers() const {
  glfwSwapBuffers(window_);
}

void Window::SetTitle(std::string title) const {
  glfwSetWindowTitle(window_, title.c_str());
}

void Window::ShowCursor() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::HideCursor() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::ToggleCursor() {
  if (IsCursorHidden()) {
    ShowCursor();
  } else {
    HideCursor();
  }
}

bool Window::IsCursorHidden() const {
  return glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void Window::InitOpenGL() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, false);
}

void Window::CreateWindow() {
  window_ = glfwCreateWindow(width_, height_, "", nullptr, nullptr);
  glfwMakeContextCurrent(window_);
  glfwSetWindowUserPointer(window_, this);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }
}

void Window::MoveToCenter() {
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  if (monitor == NULL) {
    std::cerr << "Failed to get primary monitor" << std::endl;
    return;
  }

  int screenWidth, screeHeight;
  glfwGetMonitorWorkarea(monitor, NULL, NULL, &screenWidth, &screeHeight);
  glfwSetWindowPos(window_, (screenWidth - width_) / 2, (screeHeight - height_) / 2);
}

void Window::SetupCallbacks() {
  glfwSetKeyCallback(window_,
                     [](GLFWwindow *const window, int const key, int const scancode, int const action, int const mods) {
                       auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));

                       InputEvent::InputEvent event;
                       if (action == GLFW_PRESS) {
                         event = InputEvent::KeyPressed{ static_cast<KeyCode>(key) };
                       } else if (action == GLFW_RELEASE) {
                         event = InputEvent::KeyReleased{ static_cast<KeyCode>(key) };
                       }

                       self->events_.push_back(event);
                     });

  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    WindowEvent::Resized event{ width, height };
    self->events_.push_back(event);
  });

  glfwSetCursorPosCallback(window_, [](GLFWwindow *window, double x, double y) {
    auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    InputEvent::MouseMoved event{ x, y };
    self->events_.push_back(event);
  });

  glfwSetMouseButtonCallback(window_, [](GLFWwindow *window, int button, int action, int mode) {
    auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    InputEvent::InputEvent event;
    if (action == GLFW_PRESS) {
      event = InputEvent::MouseButtonPressed{ static_cast<MouseButton>(button) };
    } else if (action == GLFW_RELEASE) {
      event = InputEvent::MouseButtonReleased{ static_cast<MouseButton>(button) };
    }
    self->events_.push_back(event);
  });

  glfwSetWindowCloseCallback(window_, [](GLFWwindow *window) {
    auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    self->events_.push_back(WindowEvent::CloseRequested());
  });
}

}  // namespace sigma
