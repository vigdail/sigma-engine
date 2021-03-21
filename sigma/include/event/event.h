#pragma once

#include "pch.h"
#include <string>
#include <vector>

namespace sigma {

template <typename T>
struct EventBus {
  std::vector<T> events;
};

namespace WindowEvent {

struct Resized {
  int width;
  int height;
};

struct Moved {
  int x;
  int y;
};

struct CloseRequested {};

struct Destroyed {};

struct Focused {
  bool focused;
};

using WindowEvent = std::variant<Resized, Moved, CloseRequested, Destroyed, Focused>;

}  // namespace WindowEvent

struct UIEvent {
  //
};

namespace input_event {

struct KeyPressed {
  //
};

struct KeyReleased {
  //
};
struct MouseButtonPressed {};
struct MouseButtonReleased {};

struct MouseMoved {
  float delta_x;
  float delta_y;
};

struct MouseWheelMoved {
  float delta;
};

using InputEvent =
  std::variant<KeyPressed, KeyReleased, MouseMoved, MouseButtonPressed, MouseButtonReleased, MouseWheelMoved>;

}  // namespace input_event

using Event = std::variant<WindowEvent::WindowEvent, UIEvent, input_event::InputEvent>;

}  // namespace sigma
