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
  Application(Ref<State> state, GameDataBuilder data_builder)
      : data_(data_builder.build()), states_(StateMachine(state)) {
    world_.addResource<EventBus<Event>>();
  }

  virtual ~Application() = default;

  void run() {
    std::cout << "[LOG] Application start" << std::endl;

    data_.start(world_);
    states_.start(StateData(world_, data_));

    while (states_.running()) {
      data_.update(world_);
      states_.update(StateData(world_, data_));

      auto& events = world_.resource<EventBus<Event>>().events;

      for (Event e : events) {
        states_.handleEvent(StateData(world_, data_), e);
      }
      events.clear();
    }

    std::cout << "[LOG] Application shutdown" << std::endl;
  }
  World& getWorld() {
    return world_;
  }

 private:
  World world_;
  GameData data_;
  StateMachine states_;
  std::vector<Event> events_;
};

}  // namespace sigma
