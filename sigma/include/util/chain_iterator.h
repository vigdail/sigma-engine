#pragma once

template<typename T>
struct type_is { using type = T; };

template<typename... T>
struct nondecay_common_type;

template<typename... T>
using nondecay_common_type_t = typename nondecay_common_type<T...>::type;

template<typename T>
struct nondecay_common_type<T> : type_is<T> {};

template<typename T>
T makeT();

template<typename T, typename... U>
struct nondecay_common_type<T, U...>
    : type_is<decltype(true ? makeT<T>() : std::declval<nondecay_common_type_t<U...>>())> {
};

/// NOTICE: This will lead to crash if begin > end in at least one provided pair
template<typename ... Iterators>
class Chain {
  template<typename C>
  using IterPairT = std::pair<C, C>;
  using Sequence = std::make_index_sequence<sizeof...(Iterators)>;
  using DerefType = nondecay_common_type_t<typename Iterators::value_type...>;
 public:
  Chain() = default;

  Chain(IterPairT<Iterators>&& ... i)
      : iterators_(i...) {}

  class Iterator {
   public:
    Iterator(IterPairT<Iterators>... pairs)
        : iter_pairs_(pairs...) {}

    DerefType operator*() {
      return dereference(Sequence{});
    }

    Iterator& operator++() {
      increment(Sequence{});
      return *this;
    }

    Iterator operator++(int) {
      Iterator tmp(*this);
      ++*this;
      return tmp;
    }

    bool operator==(Iterator const& rhs) {
      return iter_pairs_ == rhs.iter_pairs_;
    }

    bool operator!=(Iterator const& rhs) {
      return iter_pairs_ != rhs.iter_pairs_;
    }

   private:
    std::tuple<IterPairT<Iterators>...> iter_pairs_;

   private:
    void increment(std::index_sequence<>) {
      std::cout << "INC UNREACHABLE\n";
    }

    template<size_t I, size_t... Is>
    void increment(std::index_sequence<I, Is...>) {
      auto& cur_pair = std::get<I>(iter_pairs_);
      if (cur_pair.first != cur_pair.second) {
        ++cur_pair.first;
      } else {
        increment(std::index_sequence<Is...>{});
      }
    }

    DerefType dereference(std::index_sequence<>) {
      return {};
    }

    template<size_t I, size_t... Is>
    DerefType dereference(std::index_sequence<I, Is...>) {
      auto& cur_pair = std::get<I>(iter_pairs_);
      if (cur_pair.first != cur_pair.second) {
        return *cur_pair.first;
      } else {
        return dereference(std::index_sequence<Is...>{});
      }
    }
  };

  Iterator begin() {
    return beginImpl(Sequence{});
  }

  Iterator end() {
    return endImpl(Sequence{});
  }

 private:
  std::tuple<IterPairT<Iterators> ...> iterators_;

 private:
  template<size_t... Is>
  Iterator beginImpl(std::index_sequence<Is...>) {
    return Iterator{std::make_pair(
        std::get<Is>(iterators_).first,
        std::get<Is>(iterators_).second
    )...};
  }

  template<size_t... Is>
  Iterator endImpl(std::index_sequence<Is...>) {
    return Iterator{std::make_pair(
        std::get<Is>(iterators_).second,
        std::get<Is>(iterators_).second
    )...};
  }
};