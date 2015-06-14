#include "PointLight.h"


PointLight::PointLight() : Light()
{
}

PointLight::PointLight(const glm::vec4& diffuse) : Light(diffuse)
{
}

PointLight::PointLight(const glm::vec3& position, const glm::vec4& diffuse)
  : Light(diffuse), position_(position)
{
}

PointLight::~PointLight()
{
}

void PointLight::setPosition(const glm::vec3& position)
{
  position_ = position;
}

const glm::vec3& PointLight::getPosition() const
{
  return position_;
}