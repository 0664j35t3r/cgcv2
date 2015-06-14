#ifndef CAMERA_H
#define CAMERA_H
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

#define M_PI 3.14159265358979323846

class Camera
{
public:
  Camera();
  ~Camera();

  void orientate(float horizontal_angle, float vertical_angle);
  void orientate(const glm::vec2& speed);
  void move(float right, float up, float forward);
  void move(const glm::vec3& speed);
  void update(float passed_seconds);
  void zoom(float factor);

  void setOrientation(float horizontal_angle, float vertical_angle);
  void setPosition(const glm::vec3& position);
  void resetOrientation();
  void setFieldOfView(float fov);
  void setNearClipDistance(float distance_);
  void setFarClipDistance(float distance_);
  void setAspectRatio(float aspect);
  void setProjection(float fov, float aspect, float near_clip, float far_clip);
  const glm::mat4& getViewMatrix() const;
  const glm::mat4& getProjMatrix() const;

private:
  glm::vec2 angles_;
  glm::vec2 accumulated_orientation_;
  glm::vec3 direction_, right_, up_;
  glm::vec3 position_;
  glm::vec3 accumulated_movement_;
  glm::mat4 view_mat_, proj_mat_;
  float fov_, near_clip_, far_clip_, aspect_;

  void updateDirection();
  void updateRight();
  void updateUp();
  void updateViewMatrix();
  void updateProjectionMatrix();
};

#endif // CAMERA_H
