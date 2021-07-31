#pragma once

#include <filesystem>
#include <fstream>

namespace sigma {

template<class... Ts>
struct overloaded : Ts ... {
  using Ts::operator()...;
};
template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

std::string loadFile(const std::filesystem::path& filepath);

}  // namespace sigma
