/*
 * Keyframe.h
 *
 *  Created on: 26.02.2015
 *      Author: knuke
 */

#ifndef KEYFRAME_H_
#define KEYFRAME_H_

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>


class Keyframe
{
 public:
  Keyframe();

  Keyframe relativeTo(const Keyframe& other) const;
  float getTime() const;
  void setTime(float time);

  const glm::vec3& getTranslation(size_t joint) const;
  void setTranslation(size_t joint, const glm::vec3& translation);

  const glm::quat& getRotation(size_t joint) const;
  void setRotation(size_t joint, const glm::quat& rotation);

 private:
  float time_;
  std::vector<glm::vec3> translations_;
  std::vector<glm::quat> rotations_;
};

#endif /* KEYFRAME_H_ */
