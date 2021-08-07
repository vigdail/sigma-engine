#pragma once

#include <vector>
#include <queue>
#include "util/chain_iterator.h"

namespace sigma {

using ReaderHandle = std::size_t;

template<typename T>
class Events {
 public:

  class EventReader {
    std::size_t last_event_count_;
    const Events<T>& events_;
   public:
    EventReader(std::size_t last_event_count, const Events<T>& events) : last_event_count_{last_event_count},
                                                                         events_{events} {}

    // TODO: refactor this
    auto events() {
      auto a_index = (last_event_count_ > events_.a_start_event_count_)
                     ? last_event_count_ - events_.a_start_event_count_
                     : 0;
      auto b_index = (last_event_count_ > events_.b_start_event_count_)
                     ? last_event_count_ - events_.b_start_event_count_
                     : 0;

      last_event_count_ = events_.event_count_;

      auto a_start = events_.events_a_.begin() + a_index;
      auto a = std::make_pair(a_start, events_.events_a_.end());
      if (a_start >= events_.events_a_.end()) {
        a = {};
      }
      auto b_start = events_.events_b_.begin() + b_index;
      auto b = std::make_pair(b_start, events_.events_b_.end());
      if (b_start >= events_.events_b_.end()) {
        b = {};
      }
      switch (events_.state_) {
        case State::A:return Chain(std::move(b), std::move(a));
        case State::B:return Chain(std::move(a), std::move(b));
      }
    }
  };

 private:
  enum class State {
    A,
    B,
  };

 public:
  void send(T event) {
    switch (state_) {
      case State::A: {
        events_a_.emplace_back(event);
        break;
      }
      case State::B: {
        events_b_.emplace_back(event);
        break;
      }
    }

    event_count_++;
  }

  auto read(std::size_t reader_handle) {
    assert(reader_handle < readers_.size());
    auto& reader = readers_[reader_handle];

    return reader.events();
  }

  void update() {
    switch (state_) {
      case State::A: {
        events_b_ = std::vector<T>();
        state_ = State::B;
        b_start_event_count_ = event_count_;
        break;
      }
      case State::B: {
        events_a_ = std::vector<T>();
        state_ = State::A;
        a_start_event_count_ = event_count_;
        break;
      }
    }
  }

  ReaderHandle registerReader() {
    readers_.push_back({0, *this});
    return readers_.size() - 1;
  }

 private:
  std::vector<T> events_a_{};
  std::vector<T> events_b_{};
  std::size_t a_start_event_count_{0};
  std::size_t b_start_event_count_{0};
  std::size_t event_count_{0};
  State state_{State::A};
  std::vector<EventReader> readers_;
};

}

