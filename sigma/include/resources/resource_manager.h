#pragma once

#include <filesystem>
#include <util/non_copyable.h>

namespace sigma {

template<typename T>
struct AssetHandle {
  int id{-1};

  operator bool() const {
    return id >= 0;
  }
};

template<typename T>
class ResourceManager final : NonCopyable {
 public:

  AssetHandle<T> request(const std::filesystem::path& path) {
    if (auto it = resources_.find(path); it != resources_.end()) {
      return it->second.first;
    }

    std::shared_ptr<T> resource = std::make_shared<T>();
    if (!resource->loadFromFile(path)) {
      return AssetHandle<T>{-1};
    }

    auto handle = AssetHandle<T>{curret_id_++};
    resources_.insert({path, {handle, resource}});

    return handle;
  }

  std::shared_ptr<T> get(AssetHandle<T> id) const {
    for (const auto&[path, entry]: resources_) {
      const auto&[handle, resource] = entry;
      if (handle == id) {
        return resource;
      }
    }

    return nullptr;
  }

  bool has(AssetHandle<T> id) const {
    return get(id) != nullptr;
  }

  void remove(AssetHandle<T> id) {
    for (const auto&[path, entry]: resources_) {
      const auto&[handle, resource] = entry;
      if (handle == id) {
        resources_.erase(path);
        return;
      }
    }

    // TODO: log that we try remove item that already was removed
  }

 private:
  int curret_id_{0};
  std::unordered_map<std::string, std::pair<AssetHandle<T>, std::shared_ptr<T>>> resources_;
};

}