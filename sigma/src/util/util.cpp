#include "util/util.h"

namespace sigma {

std::string loadFile(const std::filesystem::path& filepath) {
  std::stringstream ss;
  std::ifstream file(filepath);
  if (!file.is_open()) {
    // throw exception
    assert(false && "TODO");
  }

  ss << file.rdbuf();
  return ss.str();
}

}