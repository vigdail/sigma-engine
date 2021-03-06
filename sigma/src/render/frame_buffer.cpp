
#include "render/frame_buffer.h"
#include <utility>

namespace sigma {

FrameBuffer::FrameBuffer(FrameBuffer::Descriptor descriptor) noexcept
    : descriptor_(std::move(descriptor)) {
  glGenFramebuffers(1, &id_);

  glBindFramebuffer(GL_FRAMEBUFFER, id_);

  if (!descriptor_.color_formats.empty()) {
    color_attachments_.reserve(descriptor_.color_formats.size());
  }

  auto texture_target = [](bool multisampled) -> uint32_t {
    return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
  };

  std::vector<uint32_t> atts(color_attachments_.size());
  uint32_t index = 0;
  for (auto format : descriptor_.color_formats) {
    TextureViewDescriptor view{};
    view.width = descriptor_.width;
    view.height = descriptor_.height;
    view.internal_format = format;

    TextureHandle texture = TextureBuilder().withView(view).build();

    auto att = GL_COLOR_ATTACHMENT0 + index++;
    atts.push_back(att);
    glFramebufferTexture2D(GL_FRAMEBUFFER, att,
                           texture_target(descriptor_.is_multisampled),
                           texture->getId(), 0);

    color_attachments_.push_back(std::move(texture));
  }

  if (descriptor_.depth_format != 0) {
    TextureViewDescriptor view{};
    view.width = descriptor_.width;
    view.height = descriptor_.height;
    view.image_format = GL_DEPTH_COMPONENT;
    view.internal_format = descriptor_.depth_format;
    view.type = GL_FLOAT;

    TextureHandle texture = TextureBuilder().withView(view).build();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           texture_target(descriptor_.is_multisampled),
                           texture->getId(), 0);

    depth_attachment_ = texture;
  }

  glDrawBuffers(color_attachments_.size(), atts.data());

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : id_(std::exchange(other.id_, 0)), descriptor_(std::move(other.descriptor_)),
      color_attachments_(std::move(other.color_attachments_)),
      depth_attachment_(std::move(other.depth_attachment_)) {}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept {
  if (this != &other) {
    drop();
    std::swap(id_, other.id_);
    std::swap(descriptor_, other.descriptor_);
    std::swap(color_attachments_, other.color_attachments_);
    std::swap(depth_attachment_, other.depth_attachment_);
  }

  return *this;
}

FrameBuffer::~FrameBuffer() { drop(); }

void FrameBuffer::drop() {
  glDeleteFramebuffers(1, &id_);
  id_ = 0;
  color_attachments_.clear();
  depth_attachment_ = {};
}

void FrameBuffer::bind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, id_);
  glViewport(0, 0, descriptor_.width, descriptor_.height);
}

void FrameBuffer::unbind() const {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bindRead() const {
  glBindFramebuffer(GL_READ_FRAMEBUFFER, id_);
}

int FrameBuffer::getWidth() const {
  return descriptor_.width;
}

int FrameBuffer::getHeight() const {
  return descriptor_.height;
}

}