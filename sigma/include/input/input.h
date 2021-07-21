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
  Input() : key_states_(), mouse_button_states_(), mouse_position_(glm::vec2(0.0f)) {
  }
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
    auto& bus = world.resource<sigma::EventBus<sigma::Event>>();

    for (auto& e : bus.events) {
      std::visit(overloaded{
                     [&](sigma::input_event::InputEvent ev) { onEvent(world, ev); },
                     [](auto) {},
                 },
                 e);
    }
  }

 private:
  void onEvent(World& world, input_event::InputEvent event) {
    auto& input = world.resource<Input>();
    std::visit(overloaded{
                   [&](sigma::input_event::KeyPressed e) { input.onKeyPressed(e.key); },
                   [&](sigma::input_event::KeyReleased e) { input.onKeyReleased(e.key); },
                   [&](sigma::input_event::MouseButtonPressed e) { input.onMouseButtonPressed(e.button); },
                   [&](sigma::input_event::MouseButtonReleased e) { input.onMouseButtonReleased(e.button); },
                   [&](sigma::input_event::MouseMoved e) { input.onMouseMoved(e.x, e.y); },
                   [&](sigma::input_event::MouseWheelMoved e) { /* @TODO */ },
                   [](auto) {},
               },
               event);
  }
};

}  // namespace sigma
