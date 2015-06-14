#include "Joint.h"
#include <glm/gtc/matrix_inverse.hpp>


Joint::Joint()
  : id_(0)
  , offset_(0)
  , rotation_()
  , base_pose_matrix_(1)
  , inverse_base_pose_matrix_(1)
  , parent_(-1)
{
}

Joint::Joint(const Joint& joint)
  : id_(joint.id_)
  , offset_(joint.offset_)
  , rotation_(joint.rotation_)
  , base_pose_matrix_(joint.base_pose_matrix_)
  , inverse_base_pose_matrix_(joint.inverse_base_pose_matrix_)
  , parent_(joint.parent_)
{

}

Joint::~Joint()
{
}

void Joint::setID(const unsigned id)
{
  id_ = id;
}

unsigned Joint::getID() const
{
  return id_;
}

void Joint::setOffset(const glm::vec3& offset)
{
  offset_ = offset;
}

const glm::vec3& Joint::getOffset() const
{
  return offset_;
}

void Joint::setParent(int parent)
{
  parent_ = parent;
}

void Joint::setRotation(const glm::quat& rotation)
{
  rotation_ = rotation;
}

const glm::quat& Joint::getRotation() const
{
  return rotation_;
}

int Joint::getParent() const
{
  return parent_;
}

bool Joint::hasParent() const
{
  return parent_ >= 0;
}

void Joint::setBasePoseMatrix(const glm::mat4& matrix)
{
  base_pose_matrix_ = matrix;
  inverse_base_pose_matrix_ = glm::inverse(matrix);
}

const glm::mat4& Joint::getBasePoseMatrix() const
{
  return base_pose_matrix_;
}

const glm::mat4& Joint::getInverseBindPoseMatrix() const
{
  return inverse_base_pose_matrix_;
}