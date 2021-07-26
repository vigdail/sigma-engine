#pragma once

#include "texture.h"
#include <optional>
#include <vector>

namespace sigma {

class FrameBuffer {
 public:
  struct Descriptor {
    uint32_t width;
    uint32_t height;
    bool is_multisampled = false;
    std::vector<uint32_t> color_formats;
    uint32_t depth_format;
  };

  FrameBuffer() noexcept = default;
  explicit FrameBuffer(Descriptor descriptor) noexcept;
  FrameBuffer(const FrameBuffer&) = delete;
  FrameBuffer(FrameBuffer&&) noexcept;
  FrameBuffer& operator=(const FrameBuffer&) = delete;
  FrameBuffer& operator=(FrameBuffer&&) noexcept;
  virtual ~FrameBuffer();

  TextureHandle getTexture(int i) { return color_attachments_[i]; }

  TextureHandle getDepth() { return depth_attachment_; }

  void bind() const;
  void unbind() const;
  void bindRead() const;
  int getWidth() const;
  int getHeight() const;

 protected:
  uint32_t id_{};
  Descriptor descriptor_;
  std::vector<TextureHandle> color_attachments_;
  TextureHandle depth_attachment_;

 private:
  void drop();
};

}
