#pragma once

#include "core/state.h"
#include "core/system.h"
#include "event/event.h"
#include "event/keycodes.h"
#include "pch.h"

#include <memory>
#include <unordered_map>

namespace sigma {

enum class KeyState {
  RELEASED,
  UP,
  DOWN,
  PRESSED,
};

class Input final {
 public:
  void update();
  KeyState getKeyState(KeyCode key) const;
  KeyState getMouseButtonState(MouseButton button) const;
  bool isPressedOrDown(KeyCode key) const;
  glm::vec2 getMousePosition() const;
  glm::vec2 getMouseMove() const;
  void onKeyPressed(KeyCode key);
  void onKeyReleased(KeyCode key);
  void onMouseMoved(float x, float y);
  void onMouseButtonPressed(MouseButton button);
  void onMouseButtonReleased(MouseButton button);

 private:
  std::unordered_map<KeyCode, KeyState> key_states_;
  std::unordered_map<MouseButton, KeyState> mouse_button_states_;
  glm::vec2 mouse_position_;
  glm::vec2 mouse_move_;

 private:
  void cycleKeyState(KeyState& state);
};

class InputSystem : public System {
 public:
  void start(World& world) override {
    world.addResource<Input>();

    // @TODO: refactor this in some way
    readers_.insert({"key_pressed", world.eventBus<input_event::KeyPressed>().registerReader()});
    readers_.insert({"key_released", world.eventBus<input_event::KeyReleased>().registerReader()});
    readers_.insert({"mouse_button_pressed", world.eventBus<input_event::MouseButtonPressed>().registerReader()});
    readers_.insert({"mouse_button_released", world.eventBus<input_event::MouseButtonReleased>().registerReader()});
    readers_.insert({"mouse_moved", world.eventBus<input_event::MouseMoved>().registerReader()});
    readers_.insert({"mouse_wheel_moved", world.eventBus<input_event::MouseWheelMoved>().registerReader()});
  }

  void update(World& world) override {
    auto& input = world.resource<Input>();
    input.update();

    onEvent<input_event::KeyPressed>(world, "key_pressed", [&](const auto& e) {
      input.onKeyPressed(e.key);
    });
    onEvent<input_event::KeyReleased>(world, "key_released", [&](const auto& e) {
      input.onKeyReleased(e.key);
    });
    onEvent<input_event::MouseButtonPressed>(world, "mouse_button_pressed", [&](const auto& e) {
      input.onMouseButtonPressed(e.button);
    });
    onEvent<input_event::MouseButtonReleased>(world, "mouse_button_released", [&](const auto& e) {
      input.onMouseButtonReleased(e.button);
    });
    onEvent<input_event::MouseMoved>(world, "mouse_moved", [&](const auto& e) {
      input.onMouseMoved(e.x, e.y);
    });
    onEvent<input_event::MouseWheelMoved>(world, "mouse_wheel_moved", [&](const auto& e) {
      // TODO
    });
  }

 private:
  std::unordered_map<const char*, ReaderHandle> readers_;

 private:
  template<typename T>
  void onEvent(World& world, const char* reader_name, std::function<void(const T&)> f) {
    auto events = world.eventBus<T>().read(readers_.at(reader_name));
    for (const auto& e: events) {
      f(e);
    }
  };
};

}  // namespace sigma
