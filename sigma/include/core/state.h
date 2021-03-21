#pragma once

#include "game_data.h"
#include "pch.h"
#include "world.h"

#include <memory>
#include <vector>

namespace sigma {

template <class... Ts>
struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

struct StateData {
  std::shared_ptr<World> world;
  std::shared_ptr<GameData> data;
  StateData() = default;
  StateData(std::shared_ptr<World> world, std::shared_ptr<GameData> data) : world(world), data(data) {
  }
};

namespace transition {
struct None {};
struct Pop {};
struct Push {
  StateData data;
};
struct Switch {
  StateData data;
};
struct Quit {};
}  // namespace transition

using Transition = std::variant<transition::None, transition::Pop, transition::Push, transition::Quit>;

class State {
 public:
  State() {
  }
  virtual ~State() = default;
  virtual void OnStart(StateData data) {
  }
  virtual void OnStop() {
  }
  virtual void OnPause() {
  }
  virtual void OnResume() {
  }
  virtual Transition Update(StateData data) {
    return transition::None();
  }
  virtual Transition FixedUpdate(StateData data) {
    return transition::None();
  }
  virtual Transition HandleEvent(StateData data, Event event) {
    return transition::None();
  }
};

class SimpleState : public State {
 public:
  Transition HandleEvent(StateData data, Event event) override {
    Transition trans = transition::None();
    std::visit(overloaded{
                 [&trans](const WindowEvent::WindowEvent &e) {
                   std::visit(overloaded{
                                [&trans](const WindowEvent::CloseRequested &) { trans = transition::Quit(); },
                                [&trans](const WindowEvent::Destroyed &) { trans = transition::Quit(); },
                                [&trans](const WindowEvent::Resized &) {},
                                [&trans](const WindowEvent::Moved &) {},
                                [&trans](const WindowEvent::Focused &) {},
                              },
                              e);
                 },
                 [](const UIEvent &e) {
                   //
                 },
                 [](const input_event::InputEvent &e) {
                   //
                 },
               },
               event);
    return trans;
  }
};

class StateMachine {
 public:
  explicit StateMachine(std::shared_ptr<State> state) {
    running_ = false;
    states_ = { state };
  }
  bool Running() {
    return running_;
  }

  void Start(StateData data) {
    if (!running_) {
      auto state = states_.back();
      state->OnStart(data);

      running_ = true;
    }
  }

  void FixedUpdate(StateData data) {
    if (!running_) {
      return;
    }

    std::shared_ptr<State> state = states_.back();
    Transition transition = state->FixedUpdate(data);

    Transit(transition);
  }
  void Update(StateData data) {
    if (!running_) {
      return;
    }

    std::shared_ptr<State> state = states_.back();
    Transition transition = state->Update(data);

    Transit(transition);
  }

  void HandleEvent(StateData data, Event event) {
    if (!running_) {
      return;
    }

    std::shared_ptr<State> state = states_.back();
    Transition transition = state->HandleEvent(data, event);

    Transit(transition);
  }

 private:
  bool running_;
  std::vector<std::shared_ptr<State>> states_;

 private:
  void Transit(const Transition &transition) {
    std::visit(overloaded{
                 [](const transition::None t) {},
                 [](const transition::Pop t) {},
                 [](const transition::Push t) {},
                 [](const transition::Switch t) {},
                 [this](const transition::Quit t) { running_ = false; },
               },
               transition);
  }  // namespace sigma
};

}  // namespace sigma
