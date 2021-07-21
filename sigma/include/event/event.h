#pragma once

#include "event/keycodes.h"
#include "pch.h"
#include <string>
#include <vector>

namespace sigma {

template<typename T>
struct EventBus {
  std::vector<T> events;
};

namespace window_event {

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

}// namespace window_event

struct UIEvent {
  //
};

namespace input_event {

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

}// namespace input_event

using Event = std::variant<window_event::WindowEvent, UIEvent, input_event::InputEvent>;

}// namespace sigma
