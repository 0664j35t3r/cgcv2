#ifndef Joint_H_INCLUDED
#define Joint_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Joint
{
public:
  Joint();
  Joint(const Joint& j);
  virtual ~Joint();

  void setID(const unsigned id);
  unsigned getID() const;

  void setOffset(const glm::vec3& offset);
  const glm::vec3& getOffset() const;

  void setRotation(const glm::quat& rotation);
  const glm::quat& getRotation() const;

  void setParent(int parent);
  int getParent() const;
  bool hasParent() const;

  void setBasePoseMatrix(const glm::mat4& matrix);
  const glm::mat4& getBasePoseMatrix() const;
  const glm::mat4& getInverseBindPoseMatrix() const;

private:
  unsigned id_;
  glm::vec3 offset_;
  glm::quat rotation_;
  glm::mat4 base_pose_matrix_;
  glm::mat4 inverse_base_pose_matrix_;
  int parent_;
};

#endif  // Joint_H_INCLUDED
