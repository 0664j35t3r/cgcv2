#include "Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

using glm::vec3;
using glm::vec2;

Camera::Camera()
    : angles_(vec2(static_cast<float>(M_PI), 0.f))
    , accumulated_orientation_(vec2(0))
    , accumulated_movement_(vec3(0))
    , fov_(25.f * static_cast<float>(M_PI) / 180.f)
    , near_clip_(0.1f)
    , far_clip_(100.f)
{
  updateDirection();
  updateRight();
  updateUp();
  updateViewMatrix();
  updateProjectionMatrix();
}

Camera::~Camera()
{
}

void Camera::orientate(float horizontal_angle, float vertical_angle)
{
  accumulated_orientation_.x += horizontal_angle;
  accumulated_orientation_.y += vertical_angle;
}

void Camera::setOrientation(float horizontal_angle, float vertical_angle)
{
  angles_.x += horizontal_angle;
  angles_.y += vertical_angle;
}

void Camera::orientate(const glm::vec2& speed)
{
  orientate(speed[0], speed[1]);
}

void Camera::move(float right, float up, float forward)
{
  accumulated_movement_[0] += right;
  accumulated_movement_[1] += up;
  accumulated_movement_[2] += forward;
}

void Camera::move(const glm::vec3& speed)
{
  move(speed[0], speed[1], speed[2]);
}

void Camera::update(float passed_seconds)
{
  angles_ += passed_seconds * accumulated_orientation_;
  accumulated_orientation_ = vec2(0, 0);

  position_ += passed_seconds * right_ * accumulated_movement_[0];
  position_ += passed_seconds * up_ * accumulated_movement_[1];
  position_ += passed_seconds * direction_ * accumulated_movement_[2];
  accumulated_movement_ = vec3(0);

  updateDirection();
  updateRight();
  updateUp();
  updateViewMatrix();
  updateProjectionMatrix();
}

void Camera::zoom(float factor)
{
  fov_ *= factor;
  if (fov_ >= static_cast<float>(M_PI) / 180.f * 120.f)
    fov_ = static_cast<float>(M_PI) / 180.f * 120.f;
  if (fov_ <= static_cast<float>(M_PI) / 180.f * 2.f)
    fov_ = static_cast<float>(M_PI) / 180.f * 2.f;
}

void Camera::setPosition(const glm::vec3& position)
{
  position_ = position;
}

void Camera::resetOrientation()
{
  angles_ = vec2(static_cast<float>(M_PI), 0.f);
}

void Camera::setFieldOfView(float fov)
{
  fov_ = fov;
}

void Camera::setNearClipDistance(float distance_)
{
  near_clip_ = distance_;
}

void Camera::setFarClipDistance(float distance_)
{
  far_clip_ = distance_;
}

void Camera::setAspectRatio(float aspect)
{
  aspect_ = aspect;
}

void Camera::setProjection(float fov,
    float aspect,
    float near_clip,
    float far_clip)
{
  setFieldOfView(fov);
  setAspectRatio(aspect);
  setNearClipDistance(near_clip);
  setFarClipDistance(far_clip);
}

const glm::mat4& Camera::getViewMatrix() const
{
  return view_mat_;
}

const glm::mat4& Camera::getProjMatrix() const
{
  return proj_mat_;
}

void Camera::updateDirection()
{
  direction_.x = std::cos(angles_[1]) * std::sin(angles_[0]);
  direction_.y = std::sin(angles_[1]);
  direction_.z = std::cos(angles_[1]) * std::cos(angles_[0]);
}

void Camera::updateRight()
{
  right_.x = std::sin(angles_[0] - static_cast<float>(M_PI) / 2.f);
  right_.y = 0;
  right_.z = std::cos(angles_[0] - static_cast<float>(M_PI) / 2.f);
}

void Camera::updateUp()
{
  up_ = glm::cross(right_, direction_);
}

void Camera::updateViewMatrix()
{
  view_mat_ = glm::lookAt(position_, position_ + direction_, up_);
}

void Camera::updateProjectionMatrix()
{
  proj_mat_ = glm::perspective(fov_, aspect_, near_clip_, far_clip_);
}
