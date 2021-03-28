#pragma once

#include "event/event.h"
#include "game_data.h"
#include "pch.h"
#include "util/util.h"
#include "world.h"

#include <memory>
#include <vector>

namespace sigma {

struct StateData {
  World &world;
  GameData &data;
  StateData() = default;
  StateData(World &world, GameData &data) : world(world), data(data) {
  }
};

class State;

namespace transition {
struct None {};
struct Pop {};
struct Push {
  Ref<State> state;
};
struct Switch {
  Ref<State> state;
};
struct Quit {};
}  // namespace transition

using Transition =
  std::variant<transition::None, transition::Pop, transition::Push, transition::Switch, transition::Quit>;

class State {
 public:
  State() {
  }
  virtual ~State() = default;
  virtual void OnStart(StateData data) {
  }
  virtual void OnStop(StateData data) {
  }
  virtual void OnPause(StateData data) {
  }
  virtual void OnResume(StateData data) {
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
  Transition HandleEvent(StateData, Event event) override {
    Transition trans = transition::None();
    std::visit(overloaded{
                 [&trans](const WindowEvent::WindowEvent &e) {
                   std::visit(overloaded{
                                [&trans](const WindowEvent::CloseRequested &) { trans = transition::Quit(); },
                                [&trans](const WindowEvent::Destroyed &) { trans = transition::Quit(); },
                                [](const auto &e) {},
                              },
                              e);
                 },
                 [](const auto &e) {},
               },
               event);
    return trans;
  }
};

class StateMachine {
 public:
  explicit StateMachine(Ref<State> state) {
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

    Ref<State> state = states_.back();
    Transition transition = state->FixedUpdate(data);

    Transit(transition, data);
  }
  void Update(StateData data) {
    if (!running_) {
      return;
    }

    Ref<State> state = states_.back();
    Transition transition = state->Update(data);

    Transit(transition, data);
  }

  void HandleEvent(StateData data, Event event) {
    if (!running_) {
      return;
    }

    Ref<State> state = states_.back();
    Transition transition = state->HandleEvent(data, event);

    Transit(transition, data);
  }

 private:
  bool running_;
  std::vector<Ref<State>> states_;

 private:
  void Transit(const Transition &transition, StateData data) {
    std::visit(overloaded{
                 [](const transition::None &t) {},
                 [&](const transition::Pop &t) { Pop(data); },
                 [&](const transition::Push &t) { Push(t.state, data); },
                 [&](const transition::Switch &t) { Switch(t.state, data); },
                 [&](const transition::Quit &t) { Stop(data); },
               },
               transition);
  }

  void Pop(StateData data) {
    if (!running_) {
      return;
    }

    states_.back()->OnStop(data);
    states_.pop_back();

    if (states_.size() > 0) {
      states_.back()->OnResume(data);
    } else {
      running_ = false;
    }
  }

  void Push(Ref<State> state, StateData data) {
    if (!running_) {
      return;
    }
    states_.back()->OnPause(data);

    states_.push_back(state);
    state->OnStart(data);
  }

  void Switch(Ref<State> state, StateData data) {
    if (!running_) {
      return;
    }

    if (states_.size() > 0) {
      states_.back()->OnStop(data);
      states_.pop_back();
    }

    states_.push_back(state);
    state->OnStart(data);
  }

  void Stop(StateData data) {
    if (!running_) {
      return;
    }

    for (auto state : states_) {
      state->OnStop(data);
    }
    states_.clear();

    running_ = false;
  }
};

}  // namespace sigma
