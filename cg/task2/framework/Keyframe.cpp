/*
 * Keyframe.cpp
 *
 *  Created on: 26.02.2015
 *      Author: knuke
 */

#include "Keyframe.h"
#include "Joint.h"

Keyframe::Keyframe()
    : time_(0.0f)
{
}

Keyframe Keyframe::relativeTo(const Keyframe& other) const
{
  Keyframe relativeK(*this);
  for (size_t i = 0; i < translations_.size(); ++i)
  { 
    relativeK.translations_[i] -= other.translations_[i];
  }
  for (size_t i = 0; i < rotations_.size(); ++i)
  {
    relativeK.rotations_[i] = relativeK.rotations_[i] * inverse(other.rotations_[i]);
  }
  return relativeK;
}

float Keyframe::getTime() const
{
  return time_;
}

void Keyframe::setTime(float time)
{
  time_ = time;
}

const glm::vec3& Keyframe::getTranslation(size_t joint) const
{
  return translations_[joint];
}

void Keyframe::setTranslation(size_t joint, const glm::vec3& translation)
{
  if (translations_.size() < joint + 1)
    translations_.resize(64 + 2 * joint);
  translations_[joint] = translation;
}

const glm::quat& Keyframe::getRotation(size_t joint) const
{
  return rotations_[joint];
}

void Keyframe::setRotation(size_t joint, const glm::quat& rotation)
{
  if (rotations_.size() < joint + 1)
    rotations_.resize(64 + 2 * joint);
  rotations_[joint] = rotation;
}
