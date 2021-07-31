#include "render/buffer.h"

namespace sigma {

Buffer::Buffer(BufferType type, std::size_t size, BufferUsage usage) noexcept: size_{size}, type_{type}, usage_{usage} {
  glCreateBuffers(1, &id_);
}

Buffer::Buffer(BufferType type, const uint8_t* data, std::size_t size, BufferUsage usage) noexcept: Buffer{type, size,
                                                                                                           usage} {
  glBindBuffer((GLenum)type_, id_);
  glBufferData((GLenum)type_, size, data, (GLenum)usage_);
}

Buffer::Buffer(Buffer&& other) noexcept {
  id_ = other.id_;
  size_ = other.size_;
  type_ = other.type_;
  usage_ = other.usage_;

  other.id_ = 0;
  other.size_ = 0;
  other.type_ = BufferType::VERTEX;
  other.usage_ = BufferUsage::STATIC_DRAW;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept {
  drop();

  id_ = other.id_;
  size_ = other.size_;
  type_ = other.type_;
  usage_ = other.usage_;

  other.id_ = 0;
  other.size_ = 0;
  other.type_ = BufferType::VERTEX;
  other.usage_ = BufferUsage::STATIC_DRAW;

  return *this;
}

void Buffer::setSubData(const uint8_t* data, std::size_t size, std::size_t offset) {
  assert(size + offset <= size_);
  bind();
  glBufferSubData((GLenum)type_, offset, size_, data);
}

void Buffer::bind() const {
  glBindBuffer((GLenum)type_, id_);
}

void Buffer::unbind() const {
  glBindBuffer((GLenum)type_, 0);
}

void Buffer::bindBase(std::size_t index) const {
  glBindBufferBase((GLenum)type_, index, id_);
}

Buffer::~Buffer() {
  drop();
}

void Buffer::drop() {
  if (this->id_ != 0) {
    glDeleteBuffers(1, &id_);
    id_ = 0;
  }
}
}
