#pragma once

#include "entity.h"
#include "event/event.h"
#include "game_data.h"
#include "pch.h"
#include "state.h"
#include "system.h"
#include "window/window.h"
#include "world.h"

#include <memory>
#include <utility>
#include <vector>

namespace sigma {

class Application {
 public:
  Application(std::shared_ptr<State> state, GameDataBuilder dataBuilder) : data_(dataBuilder.Build()) {
    world_ = std::make_shared<World>();
    states_ = std::make_unique<StateMachine>(state);

    world_->Raw().set<EventBus<Event>>();
  }
  virtual ~Application() = default;
  void Run() {
    std::cout << "[LOG] Application start" << std::endl;

    data_->Start(world_);
    states_->Start(StateData(world_, data_));

    while (states_->Running()) {
      data_->Update(world_);
      states_->Update(StateData(world_, data_));

      auto &events = world_->Raw().ctx<EventBus<Event>>().events;

      for (Event e : events) {
        states_->HandleEvent(StateData(world_, data_), e);
      }
      events.clear();
    }

    std::cout << "[LOG] Application shutdown" << std::endl;
  }
  std::shared_ptr<World> GetWorld() {
    return world_;
  }

 private:
  std::shared_ptr<World> world_;
  std::shared_ptr<GameData> data_;
  std::unique_ptr<StateMachine> states_;
  std::vector<Event> events_;
};

}  // namespace sigma
