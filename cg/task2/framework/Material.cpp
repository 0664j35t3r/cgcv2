#include "Material.h"


Material::Material() 
  : diffuse_(glm::vec3(0))
  , alpha_(1.f) 
{ }

Material::Material(const Material& material) 
  : diffuse_(material.diffuse_)
  , alpha_(material.alpha_) 
{ }

Material::~Material() { }

void Material::setDiffuse(const glm::vec3& diffuse)
{
  diffuse_ = diffuse;
}

const glm::vec3& Material::getDiffuse() const
{
  return diffuse_;
}

void Material::setAlpha(float alpha)
{
  alpha_ = alpha;
}

float Material::getAlpha() const
{
  return alpha_;
}