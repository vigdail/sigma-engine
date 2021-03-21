#include "input/input.h"

namespace sigma {

void Input::Update() {
  for (auto &key : key_states_) {
    CycleKeyState(key.second);
  }
  for (auto &button : mouse_button_states_) {
    CycleKeyState(button.second);
  }
}

KeyState Input::GetKeyState(KeyCode key) const {
  auto it = key_states_.find(key);
  if (it != key_states_.cend()) {
    return it->second;
  }

  return KeyState::Up;
}

KeyState Input::GetMouseButtonState(MouseButton button) const {
  auto it = mouse_button_states_.find(button);
  if (it != mouse_button_states_.cend()) {
    return it->second;
  }

  return KeyState::Up;
}

glm::vec2 Input::GetMousePosition() const {
  return mouse_position_;
}
glm::vec2 Input::GetMouseMove() const {
  return mouse_move_;
}

void Input::OnKeyPressed(KeyCode key) {
  key_states_[key] = KeyState::Pressed;
}

void Input::OnKeyReleased(KeyCode key) {
  key_states_[key] = KeyState::Released;
}

void Input::OnMouseMoved(float x, float y) {
  glm::vec2 pos(x, y);
  mouse_move_ = pos - mouse_position_;
  mouse_position_ = pos;
}

void Input::OnMouseButtonPressed(MouseButton button) {
  mouse_button_states_[button] = KeyState::Pressed;
}

void Input::OnMouseButtonReleased(MouseButton button) {
  mouse_button_states_[button] = KeyState::Released;
}

void Input::CycleKeyState(KeyState &state) {
  if (state == KeyState::Pressed) {
    state = KeyState::Down;
  } else if (state == KeyState::Released) {
    state = KeyState::Up;
  }
}

}  // namespace sigma
