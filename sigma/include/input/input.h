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
  Released,
  Up,
  Down,
  Pressed,
};

class Input {
 public:
  Input() : key_states_(), mouse_button_states_(), mouse_position_(vec2(0.0f)) {
  }
  virtual ~Input() = default;

  void Update();
  KeyState GetKeyState(KeyCode key) const;
  KeyState GetMouseButtonState(MouseButton button) const;
  vec2 GetMousePosition() const;
  vec2 GetMouseMove() const;
  void OnKeyPressed(KeyCode key);
  void OnKeyReleased(KeyCode key);
  void OnMouseMoved(float x, float y);
  void OnMouseButtonPressed(MouseButton button);
  void OnMouseButtonReleased(MouseButton button);

 private:
  std::unordered_map<KeyCode, KeyState> key_states_;
  std::unordered_map<MouseButton, KeyState> mouse_button_states_;
  vec2 mouse_position_;
  vec2 mouse_move_;

 private:
  void CycleKeyState(KeyState &state);
};

class InputSystem : public System {
 public:
  void Start(World &world) override {
    world.AddResource<Input>();
  }

  void Update(World &world) override {
    auto &bus = world.Resource<sigma::EventBus<sigma::Event>>();

    for (auto &e : bus.events) {
      std::visit(overloaded{
                   [&](sigma::InputEvent::InputEvent ev) { OnEvent(world, ev); },
                   [](auto) {},
                 },
                 e);
    }
  }

 private:
  void OnEvent(World &world, InputEvent::InputEvent event) {
    auto &input = world.Resource<Input>();
    std::visit(overloaded{
                 [&](sigma::InputEvent::KeyPressed e) { input.OnKeyPressed(e.key); },
                 [&](sigma::InputEvent::KeyReleased e) { input.OnKeyReleased(e.key); },
                 [&](sigma::InputEvent::MouseButtonPressed e) { input.OnMouseButtonPressed(e.button); },
                 [&](sigma::InputEvent::MouseButtonReleased e) { input.OnMouseButtonReleased(e.button); },
                 [&](sigma::InputEvent::MouseMoved e) { input.OnMouseMoved(e.x, e.y); },
                 [&](sigma::InputEvent::MouseWheelMoved e) { /* @TODO */ },
                 [](auto) {},
               },
               event);
  }
};

}  // namespace sigma
