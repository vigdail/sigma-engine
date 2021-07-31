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

class Input {
 public:
  Input() : key_states_(), mouse_button_states_(), mouse_position_(glm::vec2(0.0f)) {}

  virtual ~Input() = default;

  void update();
  KeyState getKeyState(KeyCode key) const;
  KeyState getMouseButtonState(MouseButton button) const;
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
  }

  void update(World& world) override {
    auto& input = world.resource<Input>();
    onEvent<input_event::KeyPressed>(world, [&](const auto& e) {
      input.onKeyPressed(e.key);
    });
    onEvent<input_event::KeyReleased>(world, [&](const auto& e) {
      input.onKeyReleased(e.key);
    });
    onEvent<input_event::MouseButtonPressed>(world, [&](const auto& e) {
      input.onMouseButtonPressed(e.button);
    });
    onEvent<input_event::MouseButtonReleased>(world, [&](const auto& e) {
      input.onMouseButtonReleased(e.button);
    });
    onEvent<input_event::MouseMoved>(world, [&](const auto& e) {
      input.onMouseMoved(e.x, e.y);
    });
    onEvent<input_event::MouseWheelMoved>(world, [&](const auto& e) {
      // TODO
    });
  }

 private:
  template<typename T>
  void onEvent(World& world, std::function<void(const T&)> f) {
    for (const auto& e: world.eventBus<T>().events) {
      f(e);
    }
  };
};

}  // namespace sigma
