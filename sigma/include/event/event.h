#pragma once

#include "event/keycodes.h"
#include "pch.h"
#include <string>
#include <vector>

namespace sigma {

//template<typename T>
//struct EventBus {
//  std::vector<T> events;
//};

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

}// namespace window_event

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

}// namespace input_event

using Event = std::variant<
    window_event::Resized,
    window_event::Moved,
    window_event::CloseRequested,
    window_event::Destroyed,
    window_event::Focused,
    input_event::KeyPressed,
    input_event::KeyReleased,
    input_event::MouseMoved,
    input_event::MouseButtonPressed,
    input_event::MouseButtonReleased,
    input_event::MouseWheelMoved
>;

}// namespace sigma
