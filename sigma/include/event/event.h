#pragma once

#include "event/keycodes.h"
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

namespace InputEvent {

struct KeyPressed {
  KeyCode key;
};

struct KeyReleased {
  KeyCode key;
};
struct MouseButtonPressed {
  MouseButton button;
};
struct MouseButtonReleased {
  MouseButton button;
};

struct MouseMoved {
  float x;
  float y;
};

struct MouseWheelMoved {
  float delta;
};

using InputEvent =
  std::variant<KeyPressed, KeyReleased, MouseMoved, MouseButtonPressed, MouseButtonReleased, MouseWheelMoved>;

}  // namespace InputEvent

using Event = std::variant<WindowEvent::WindowEvent, UIEvent, InputEvent::InputEvent>;

}  // namespace sigma
