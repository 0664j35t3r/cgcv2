/*
 * Animation.h
 *
 *  Created on: 26.02.2015
 *      Author: knuke
 */

#ifndef ACTION_H_
#define ACTION_H_

#include <vector>

#include "Keyframe.h"


class Animation
{
public:
  Animation();

  void setKeyframes(const Keyframe* keyframes, size_t num_keyframes);
  void setRepeatTime(float time);
  float loopTime(float time) const;

  const std::vector<Keyframe>& getKeyframes() const;

private:
  std::vector<Keyframe> keyframes_;
  float repeat_time_;
};

#endif /* ACTION_H_ */
