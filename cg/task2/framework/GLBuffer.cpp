#include "GLBuffer.h"
#include <GL/gl3w.h>

GLBuffer::GLBuffer(BufferType type) : handle_(0), type_(type)
{
  gl_type_ = GL_ARRAY_BUFFER;
  if (type_ == INDEX_BUFFER)
    gl_type_ = GL_ELEMENT_ARRAY_BUFFER;
}

GLBuffer::~GLBuffer()
{
  destroy();
}

void GLBuffer::create()
{
  glGenBuffers(1, &handle_);
}

void GLBuffer::bind()
{
  glBindBuffer(gl_type_, handle_);
}

void GLBuffer::unbind()
{
  glBindBuffer(gl_type_, 0);
}

void GLBuffer::allocate(size_t byte_count, Usage usage)
{
  gl_usage_ = GL_STATIC_DRAW;
  if (usage == DYNAMIC_DRAW)
    gl_usage_ = GL_DYNAMIC_DRAW;
  else if (usage == STREAM_DRAW)
    gl_usage_ = GL_STREAM_DRAW;

  glBufferData(gl_type_, byte_count, 0, gl_usage_);

  size_ = byte_count;
}

void GLBuffer::discardData()
{
  glBufferData(gl_type_, size_, 0, gl_usage_);
}

void GLBuffer::write(size_t size, const void* data, int offset)
{
  glBufferSubData(gl_type_, offset, size, data);
}

void GLBuffer::write(size_t size, const float* data, int offset)
{
  glBufferSubData(gl_type_, offset, size * sizeof(float), data);
}

void GLBuffer::write(const std::vector<glm::vec3>& data, int offset)
{
  write(data.size() * 3, &data[0][0], offset);
}

void GLBuffer::write(size_t size, const int* data, int offset)
{
  glBufferSubData(gl_type_, offset, size * sizeof(int), data);
}

void GLBuffer::write(const std::vector<glm::ivec3>& data, int offset)
{
  write(data.size() * 3, &data[0][0], offset);
}

void GLBuffer::map(void** data, BufferAccessType type)
{
  *data = glMapBuffer(gl_type_, type );
}

void GLBuffer::map(float** data, BufferAccessType type)
{
  map(reinterpret_cast<void**>(data), type);
}

void GLBuffer::map(glm::vec3** data, BufferAccessType type)
{
  map(reinterpret_cast<float**>(data), type);
}

void GLBuffer::unmap()
{
  glUnmapBuffer(gl_type_);
}
void GLBuffer::destroy()
{
  if (handle_)
    glDeleteBuffers(1, &handle_);
}

