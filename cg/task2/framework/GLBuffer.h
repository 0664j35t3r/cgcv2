#ifndef GLBUFFER_H
#define GLBUFFER_H

#include <GL/gl3w.h>
#include <vector>
#include <glm/vec3.hpp>

class GLBuffer
{
public:

  enum BufferType
  {
    VERTEX_BUFFER, INDEX_BUFFER
  };

  enum BufferAccessType
  {
    READ = GL_READ_ONLY, WRITE = GL_WRITE_ONLY, READ_WRITE = GL_READ_WRITE
  };

  enum Usage
  {
    STREAM_DRAW, STATIC_DRAW, DYNAMIC_DRAW
  };

  GLBuffer(BufferType type);
  ~GLBuffer();

  void create();
  void bind();
  void unbind();
  void allocate(size_t byte_count, Usage usage);
  void discardData();
  void write(size_t size, const void* data, int offset = 0);
  void write(size_t size, const float* data, int offset = 0);
  void write(const std::vector<glm::vec3>& data, int offset = 0);
  void write(size_t size, const int* data, int offset = 0);
  void write(const std::vector<glm::ivec3>& data, int offset = 0);
  void map(void** data, BufferAccessType type = BufferAccessType::READ_WRITE);
  void map(float** data, BufferAccessType type = BufferAccessType::READ_WRITE);
  void map(glm::vec3** data, BufferAccessType type = BufferAccessType::READ_WRITE);
  void unmap();
  void destroy();

private:
  unsigned int handle_;
  BufferType type_;
  unsigned int gl_type_;
  GLenum gl_usage_;
  size_t size_;
};

#endif // GLBUFFER_H
