#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

class PointLight :
  public Light
{
public:
  PointLight();
  PointLight(const glm::vec4& diffuse);
  PointLight(const glm::vec3& position, const glm::vec4& diffuse);
  virtual ~PointLight();

  void setPosition(const glm::vec3& position);
  const glm::vec3& getPosition() const;

private:
  glm::vec3 position_;
};

#endif
