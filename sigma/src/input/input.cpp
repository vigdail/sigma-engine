#include "input/input.h"

namespace sigma {

void Input::update() {
  mouse_move_ = glm::vec2();
  for (auto& key : key_states_) {
    cycleKeyState(key.second);
  }
  for (auto& button : mouse_button_states_) {
    cycleKeyState(button.second);
  }
}

KeyState Input::getKeyState(KeyCode key) const {
  auto it = key_states_.find(key);
  if (it != key_states_.cend()) {
    return it->second;
  }

  return KeyState::UP;
}

KeyState Input::getMouseButtonState(MouseButton button) const {
  auto it = mouse_button_states_.find(button);
  if (it != mouse_button_states_.cend()) {
    return it->second;
  }

  return KeyState::UP;
}

glm::vec2 Input::getMousePosition() const {
  return mouse_position_;
}
glm::vec2 Input::getMouseMove() const {
  return mouse_move_;
}

void Input::onKeyPressed(KeyCode key) {
  key_states_[key] = KeyState::PRESSED;
}

void Input::onKeyReleased(KeyCode key) {
  key_states_[key] = KeyState::RELEASED;
}

void Input::onMouseMoved(float x, float y) {
  glm::vec2 pos(x, y);
  mouse_move_ = pos - mouse_position_;
  mouse_position_ = pos;
}

void Input::onMouseButtonPressed(MouseButton button) {
  mouse_button_states_[button] = KeyState::PRESSED;
}

void Input::onMouseButtonReleased(MouseButton button) {
  mouse_button_states_[button] = KeyState::RELEASED;
}

void Input::cycleKeyState(KeyState& state) {
  if (state == KeyState::PRESSED) {
    state = KeyState::DOWN;
  } else if (state == KeyState::RELEASED) {
    state = KeyState::UP;
  }
}

bool Input::isPressedOrDown(KeyCode key) const {
  auto state = getKeyState(key);
  return state == KeyState::PRESSED || state == KeyState::DOWN;
}

}  // namespace sigma
