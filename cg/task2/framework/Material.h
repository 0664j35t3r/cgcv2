#ifndef Material_H_INCLUDED
#define Material_H_INCLUDED

#include <glm/glm.hpp>

class Material
{
public:
  Material();
  Material(const Material& material);
  virtual ~Material();

  void setDiffuse(const glm::vec3& diffuse);
  const glm::vec3& getDiffuse() const;

  void setAlpha(float alpha);
  float getAlpha() const;

private:
  glm::vec3 diffuse_;
  float alpha_;
};

#endif  // Material_H_INCLUDED
