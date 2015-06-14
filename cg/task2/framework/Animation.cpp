/*
 * Animation.cpp
 *
 *  Created on: 26.02.2015
 *      Author: knuke
 */

#include <limits>
#include <algorithm>
#include <cmath>

#include "Animation.h"
#include "Keyframe.h"

Animation::Animation()
  : repeat_time_(std::numeric_limits<float>::infinity())
{
}

void Animation::setRepeatTime(float repeat_time)
{
  repeat_time_ = repeat_time;
}

float Animation::loopTime(float time) const
{
  if (!std::isinf(repeat_time_))
    time = repeat_time_ != 0.0f ? std::fmod(time, repeat_time_) : 0.0f;

  return std::min(std::max(time, 0.0f), keyframes_.back().getTime());
}

void Animation::setKeyframes(const Keyframe* keyframes, size_t num_keyframes)
{
  keyframes_ = std::vector<Keyframe>(keyframes, keyframes + num_keyframes);
  if (std::isinf(repeat_time_))
    repeat_time_ = keyframes_.back().getTime();
}

const std::vector<Keyframe>& Animation::getKeyframes() const
{
  return keyframes_;
}
