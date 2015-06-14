#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <vector>
#include <map>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class Shader
{
 public:
  enum ShaderType
  {
    VERTEX_SHADER,
    FRAGMENT_SHADER
  };

  Shader();
  virtual ~Shader();

  bool create();
  bool addShader(ShaderType type, const std::string& source);
  bool link();
  void destroy();
  void bind();
  void unbind();

  void setUniformMatrix4f(const std::string& name, const float* matrix);
  void setUniformMatrix4f(const std::string& name, const glm::mat4& matrix);
  void setUniform3f(const std::string& name, const float* data);
  void setUniform3f(const std::string& name, const glm::vec3& data);
  void setUniform4f(const std::string& name, const float* data);
  void setUniform4f(const std::string& name, const glm::vec4& data);
  void setUniform1i(const std::string& name, int data);
  void setAttribPointer(const std::string& name,
      unsigned int type,
      int size,
      int stride,
      size_t offset = 0);
  void enableAttribArray(const std::string& name);

 private:
  int program_;
  std::vector<int> shaders_;

  std::map<std::string, int> uniforms_;
  typedef std::map<std::string, int>::iterator uniform_map_iter;
  typedef std::pair<std::string, int> uniform_map_pair;

  std::map<std::string, int> attribs_;
  typedef std::map<std::string, int>::iterator attrib_map_iter;
  typedef std::pair<std::string, int> attrib_map_pair;

  std::string getShaderLog(int shader);
  bool checkShaderCompile(int shader);
  bool checkShaderLink(int shader);

  unsigned int getUniformLocation(const std::string& name);
  unsigned int getAttribLocation(const std::string& name);
};

#endif // SHADER_H
