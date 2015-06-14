#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

class Light
{
public:
  Light();
  Light(const glm::vec4& diffuse);
  virtual ~Light();

  void setDiffuse(const glm::vec4& diffuse);
  const glm::vec4& getDiffuse() const;

private:
  glm::vec4 diffuse_;
};

#endif
