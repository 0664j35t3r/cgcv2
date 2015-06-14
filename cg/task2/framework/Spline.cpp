#include "Spline.h"
#include "../task2.h"
#include <algorithm>

#ifndef M_PI
#define M_PI 3.141592f
#endif

SplinePoint::SplinePoint()
    : time_(0.f)
    , point_(0.f, 0.f, 0.f)
    , tangent_(0.f, 0.f, 0.f)
{
}

SplinePoint::SplinePoint(const SplinePoint& instance)
    : time_(instance.time_)
    , point_(instance.point_)
    , tangent_(instance.tangent_)
{
}

void SplinePoint::setPoint(float time, const glm::vec3& point)
{
  time_ = time;
  point_ = point;
}

void SplinePoint::setTangent(const glm::vec3& tangent)
{
  tangent_ = tangent;
}

const glm::vec3& SplinePoint::getPoint() const
{
  return point_;
}

float SplinePoint::getTime() const
{
  return time_;
}

const glm::vec3& SplinePoint::getTangent() const
{
  return tangent_;
}

SplineInterpolationResult::SplineInterpolationResult(const glm::vec3& position,
    const glm::quat& orientation)
    : position_(position)
    , orientation_(orientation)
{
}

SplineInterpolationResult::SplineInterpolationResult(
    const SplineInterpolationResult& instance)
    : position_(instance.position_)
    , orientation_(instance.orientation_)
{
}

const glm::vec3& SplineInterpolationResult::getPosition()
{
  return position_;
}

const glm::quat& SplineInterpolationResult::getOrientation()
{
  return orientation_;
}

void Spline::addPoint(float time, const glm::vec3& point)
{
  SplinePoint p;
  p.setPoint(time, point);
  points_.push_back(p);
  std::sort(points_.begin(), points_.end(), SplinePoint::compare);
}

void Spline::calculateAndStoreTangents()
{
  for (size_t point_index = 1; point_index + 1 < points_.size(); point_index++)
  {
    SplinePoint& p = points_[point_index];
    const SplinePoint& p_prev = points_[point_index - 1];
    const SplinePoint& p_next = points_[point_index + 1];

    glm::vec3 tangent = (p_next.getPoint() - p_prev.getPoint()) /
        (p_next.getTime() - p_prev.getTime());
    //    tangent = glm::normalize(tangent);

    p.setTangent(tangent);
  }
}

bool SplinePoint::compare(SplinePoint a, SplinePoint b)
{
  return a.getTime() < b.getTime();
}

Spline::Spline()
    : points_()
{
}

Spline::Spline(const Spline& instance)
    : points_(instance.points_)
{
}

SplineInterpolationResult Spline::interpolate(float time) const
{
  glm::vec3 position, tangent;
  interpolateSpline(time, &points_[0] + 1, points_.size() - 2, position, tangent);
  glm::quat orientation = lookAtToOrientation(tangent);
  return SplineInterpolationResult(position, orientation);
}

glm::quat Spline::lookAtToOrientation(const glm::vec3& look_at_direction) const
{
  glm::vec3 dir = glm::normalize(look_at_direction);
  glm::vec3 forward(0, 0, 1);
  float dot = glm::dot(forward, dir);

  if (std::abs(dot + 1.f) < 0.001f)
    return glm::angleAxis(static_cast<float>(M_PI), glm::vec3(0, 1, 0));

  if (std::abs(dot - 1.f) < 0.001f)
    return glm::quat();

  float angle = std::acos(dot);
  glm::vec3 cross = glm::normalize(glm::cross(forward, dir));
  glm::quat orientation = glm::normalize(glm::angleAxis(angle, cross));
  return orientation;
}

float Spline::getStartTime() const
{
  return points_[1].getTime();
}

float Spline::getEndTime() const
{
  return points_[points_.size() - 2].getTime();
}
