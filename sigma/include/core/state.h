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
  World& world;
  GameData& data;

  StateData(World& world, GameData& data) : world(world), data(data) {
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
}// namespace transition

using Transition =
std::variant<transition::None, transition::Pop, transition::Push, transition::Switch, transition::Quit>;

class State {
 public:
  State() {
  }

  virtual ~State() = default;

  virtual void onStart(StateData data) {
  }

  virtual void onStop(StateData data) {
  }

  virtual void onPause(StateData data) {
  }

  virtual void onResume(StateData data) {
  }

  virtual Transition update(StateData data) {
    const auto& events = data.world.resource<EventBus<window_event::CloseRequested>>().events;
    if (!events.empty()) {
      return transition::Quit();
    }

    const auto& destroyed_events = data.world.resource<EventBus<window_event::Destroyed>>().events;
    if (!destroyed_events.empty()) {
      return transition::Quit();
    }
    return transition::None();
  }

  virtual Transition fixedUpdate(StateData data) {
    return transition::None();
  }
};

class StateMachine {
 public:
  explicit StateMachine(Ref<State> state) {
    running_ = false;
    states_ = {state};
  }

  bool running() {
    return running_;
  }

  void start(StateData data) {
    if (!running_) {
      auto state = states_.back();
      state->onStart(data);

      running_ = true;
    }
  }

  void fixedUpdate(StateData data) {
    if (!running_) {
      return;
    }

    Ref<State> state = states_.back();
    Transition transition = state->fixedUpdate(data);

    transit(transition, data);
  }

  void update(StateData data) {
    if (!running_) {
      return;
    }

    Ref<State> state = states_.back();
    Transition transition = state->update(data);

    transit(transition, data);
  }

 private:
  bool running_;
  std::vector<Ref<State>> states_;

 private:
  void transit(const Transition& transition, StateData data) {
    std::visit(overloaded{
                   [](const transition::None& t) {},
                   [&](const transition::Pop& t) { pop(data); },
                   [&](const transition::Push& t) { push(t.state, data); },
                   [&](const transition::Switch& t) { switchTo(t.state, data); },
                   [&](const transition::Quit& t) { stop(data); },
               },
               transition);
  }

  void pop(StateData data) {
    if (!running_) {
      return;
    }

    states_.back()->onStop(data);
    states_.pop_back();

    if (states_.size() > 0) {
      states_.back()->onResume(data);
    } else {
      running_ = false;
    }
  }

  void push(Ref<State> state, StateData data) {
    if (!running_) {
      return;
    }
    states_.back()->onPause(data);

    states_.push_back(state);
    state->onStart(data);
  }

  void switchTo(Ref<State> state, StateData data) {
    if (!running_) {
      return;
    }

    if (states_.size() > 0) {
      states_.back()->onStop(data);
      states_.pop_back();
    }

    states_.push_back(state);
    state->onStart(data);
  }

  void stop(StateData data) {
    if (!running_) {
      return;
    }

    for (auto state : states_) {
      state->onStop(data);
    }
    states_.clear();

    running_ = false;
  }
};

}// namespace sigma
