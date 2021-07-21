#include "window/window.h"

namespace sigma {

Window::Window(int width, int height) noexcept : width_(width), height_(height) {
  initOpenGl();
  createWindow();
  moveToCenter();
  setupCallbacks();
}

Window::~Window() {
  glfwTerminate();
}

std::vector<Event> Window::pollEvents() {
  glfwPollEvents();
  return std::move(events_);
}

void Window::swapBuffers() const {
  glfwSwapBuffers(window_);
}

void Window::setTitle(std::string title) const {
  glfwSetWindowTitle(window_, title.c_str());
}

void Window::showCursor() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void Window::hideCursor() {
  glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::toggleCursor() {
  if (isCursorHidden()) {
    showCursor();
  } else {
    hideCursor();
  }
}

bool Window::isCursorHidden() const {
  return glfwGetInputMode(window_, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void Window::initOpenGl() {
  glfwInit();

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, false);
}

void Window::createWindow() {
  window_ = glfwCreateWindow(width_, height_, "", nullptr, nullptr);
  glfwMakeContextCurrent(window_);
  glfwSetWindowUserPointer(window_, this);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
  }
}

void Window::moveToCenter() {
  GLFWmonitor* monitor = glfwGetPrimaryMonitor();
  if (monitor == NULL) {
    std::cerr << "Failed to get primary monitor" << std::endl;
    return;
  }

  int screen_width, scree_height;
  glfwGetMonitorWorkarea(monitor, NULL, NULL, &screen_width, &scree_height);
  glfwSetWindowPos(window_, (screen_width - width_) / 2, (scree_height - height_) / 2);
}

void Window::setupCallbacks() {
  glfwSetKeyCallback(window_,
                     [](GLFWwindow* const window, int const key, int const scancode, int const action, int const mods) {
                       auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));

                       input_event::InputEvent event;
                       if (action == GLFW_PRESS) {
                         event = input_event::KeyPressed{static_cast<KeyCode>(key)};
                       } else if (action == GLFW_RELEASE) {
                         event = input_event::KeyReleased{static_cast<KeyCode>(key)};
                       }

                       self->events_.push_back(event);
                     });

  glfwSetFramebufferSizeCallback(window_, [](GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);

    auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    window_event::Resized event{width, height};
    self->events_.push_back(event);
  });

  glfwSetCursorPosCallback(window_, [](GLFWwindow *window, double x, double y) {
    auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    input_event::MouseMoved event{static_cast<float>(x), static_cast<float>(y)};
    self->events_.push_back(event);
  });

  glfwSetMouseButtonCallback(window_, [](GLFWwindow *window, int button, int action, int mode) {
    auto self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    input_event::InputEvent event;
    if (action == GLFW_PRESS) {
      event = input_event::MouseButtonPressed{static_cast<MouseButton>(button)};
    } else if (action == GLFW_RELEASE) {
      event = input_event::MouseButtonReleased{static_cast<MouseButton>(button)};
    }
    self->events_.push_back(event);
  });

  glfwSetWindowCloseCallback(window_, [](GLFWwindow *window) {
    auto self = static_cast<Window *>(glfwGetWindowUserPointer(window));
    self->events_.push_back(window_event::CloseRequested());
  });
}

}  // namespace sigma
