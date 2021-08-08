#pragma once

#include <event/events.h>
#include "resources/resource_manager.h"
#include "render/texture.h"
#include "entity.h"
#include "event/event.h"
#include "game_data.h"
#include "pch.h"
#include "state.h"
#include "system.h"
#include "window/window.h"
#include "world.h"

namespace sigma {

class Application {
 public:
  Application(Ref<State> state, GameDataBuilder data_builder)
      : data_(data_builder.build()), states_(StateMachine(state)) {
    world_.addResource<ResourceManager<Texture>>();
  }

  virtual ~Application() = default;

  void run() {
    std::cout << "[LOG] Application start" << std::endl;

    data_.start(world_);
    states_.start(StateData(world_, data_));

    while (states_.running()) {
      data_.update(world_);
      states_.update(StateData(world_, data_));

      world_.updateEvents();
    }

    std::cout << "[LOG] Application shutdown" << std::endl;
  }

 private:
  World world_;
  GameData data_;
  StateMachine states_;
};

}  // namespace sigma
