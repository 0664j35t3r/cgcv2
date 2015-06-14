#include "Light.h"


Light::Light() : diffuse_(0, 0, 0, 1)
{
}

Light::Light(const glm::vec4& diffuse) : diffuse_(diffuse)
{
}

Light::~Light()
{
}

void Light::setDiffuse(const glm::vec4& diffuse)
{
  diffuse_ = diffuse;
}

const glm::vec4& Light::getDiffuse() const
{
  return diffuse_;
}