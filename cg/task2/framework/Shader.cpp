#include <GL/gl3w.h>
#include "Shader.h"
#include <iostream>
#include <cstdlib>

using std::string;
using std::endl;
using std::cout;
using std::cerr;
using std::map;
using std::pair;

Shader::Shader()
    : program_(0)
{
}

Shader::~Shader()
{
  destroy();
}

bool Shader::create()
{
  program_ = glCreateProgram();

  if (!program_)
  {
    cerr << "Shader program creation failed." << endl;
    return false;
  }

  return true;
}

bool Shader::addShader(Shader::ShaderType type, const std::string& source)
{
  int shaderType = GL_VERTEX_SHADER;
  if (type == FRAGMENT_SHADER)
    shaderType = GL_FRAGMENT_SHADER;

  int shader = glCreateShader(shaderType);

  const char* source_cstr = source.c_str();
  glShaderSource(shader, 1, &source_cstr, 0);
  glCompileShader(shader);

  if (!checkShaderCompile(shader))
  {
    glDeleteShader(shader);
    return false;
  }

  shaders_.push_back(shader);
  glAttachShader(program_, shader);
  return true;
}

bool Shader::link()
{
  glLinkProgram(program_);
  if (!checkShaderLink(program_))
  {
    glDeleteProgram(program_);
    return false;
  }
  return true;
}

void Shader::destroy()
{
  for (int shader : shaders_)
  {
    glDeleteShader(shader);
    cout << "Shader (" << shader << ") deleted." << endl;
  }
  shaders_.clear();

  if (program_)
  {
    glDeleteProgram(program_);
    cout << "Shader Program (" << program_ << ") deleted." << endl;
  }
}

void Shader::bind()
{
  glUseProgram(program_);
}

void Shader::unbind()
{
  glUseProgram(0);
}

void Shader::setUniformMatrix4f(const std::string& name, const float* matrix)
{
  unsigned int loc = getUniformLocation(name);
  glUniformMatrix4fv(loc, 1, false, matrix);
}

void Shader::setUniformMatrix4f(const std::string& name,
    const glm::mat4& matrix)
{
  unsigned int loc = getUniformLocation(name);
  glUniformMatrix4fv(loc, 1, false, &matrix[0][0]);
}

void Shader::setUniform3f(const std::string& name, const float* data)
{
  unsigned int loc = getUniformLocation(name);
  glUniform3fv(loc, 1, data);
}

void Shader::setUniform3f(const std::string& name, const glm::vec3& data)
{
  setUniform3f(name, &data[0]);
}

void Shader::setUniform4f(const std::string& name, const float* data)
{
  unsigned int loc = getUniformLocation(name);
  glUniform4fv(loc, 1, data);
}

void Shader::setUniform4f(const std::string& name, const glm::vec4& data)
{
  setUniform4f(name, &data[0]);
}

void Shader::setUniform1i(const std::string& name, int data)
{
  unsigned int loc = getUniformLocation(name);
  glUniform1i(loc, data);
}

void Shader::setAttribPointer(const std::string& name,
    unsigned int type,
    int size,
    int stride,
    size_t offset)
{
  unsigned int loc = getAttribLocation(name);
  glVertexAttribPointer(loc, size, type, GL_FALSE, stride, (void*)offset);
}

void Shader::enableAttribArray(const std::string& name)
{
  unsigned int loc = getAttribLocation(name);
  glEnableVertexAttribArray(loc);
}

std::string Shader::getShaderLog(int shader)
{
  GLint log_len = 0;
  glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_len);
  if (log_len)
  {
    char* log = (char*)malloc(log_len);
    glGetShaderInfoLog(shader, log_len, 0, log);
    return string(log);
  }
  return string();
}

bool Shader::checkShaderCompile(int shader)
{
  GLint compiled;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
  if (compiled)
  {
    cout << "Shader (" << shader << ") compiled without errors." << endl;
    return true;
  }
  string log = getShaderLog(shader);
  cerr << "Shader (" << shader << ") compilation failed with the "
                                  "following errors:" << endl
       << log << endl;
  return false;
}

bool Shader::checkShaderLink(int shader)
{
  int linked = 0;
  glGetProgramiv(shader, GL_LINK_STATUS, &linked);
  if (linked)
  {
    cout << "Shader program (" << shader << ") linked without errors." << endl;
    return true;
  }
  string log = getShaderLog(shader);
  cerr << "Shader program ("
       << ") linking failed with the following "
          "errors:" << endl
       << log << endl;
  return false;
}

unsigned int Shader::getAttribLocation(const std::string& name)
{
  int loc;
  attrib_map_iter it = attribs_.find(name);
  if (it == attribs_.end())
  {
    loc = glGetAttribLocation(program_, name.c_str());
    attribs_.insert(attrib_map_pair(name, loc));
  }
  else
  {
    loc = it->second;
  }
  return loc;
}

unsigned int Shader::getUniformLocation(const std::string& name)
{
  int loc;
  uniform_map_iter it = uniforms_.find(name);
  if (it == uniforms_.end())
  {
    loc = glGetUniformLocation(program_, name.c_str());
    uniforms_.insert(uniform_map_pair(name, loc));
  }
  else
  {
    loc = it->second;
  }
  return loc;
}
