#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include "tinyxml2.h"
#include "Spline.h"

class Config
{
 public:
  Config();
  virtual ~Config();

  const std::string& getModelFileName() const;
  const std::vector<std::string>& getAnimationFileNames() const;
  const std::vector<bool>& getAnimationRelativeFlags() const;
  const std::vector<float>& getAnimationRepeatTime() const;
  const glm::vec3 getCameraPosition() const;
  float getCameraVerticalAngle() const;
  float getCameraHorizontalAngle() const;
  float getCameraFOV() const;
  float getCameraSpeed() const;
  int getRenderMode() const;
  bool exportJointTransformations() const;
  const std::string& getJointTransformationsFileName() const;
  bool useTransformationsFile() const;
  bool hasSpline() const;
  const Spline& getSpline() const;
  bool hasScreenshotFrames() const;
  const std::vector<float>& getScreenshotFrames() const;
  const std::string& getScreenshotsFolder() const;
  bool hasAnimationBlending() const;
  unsigned getAnimationBlendingFrom() const;
  unsigned getAnimationBlendingTo() const;
  float getBoneSize() const;
  float getJointSize() const;

  bool load(const std::string& file_name);

 private:
  std::string model_file_name_;
  std::vector<std::string> animation_file_names_;
  std::vector<bool> animation_relative_;
  std::vector<float> animation_repeat_;
  glm::vec3 camera_position_;
  float camera_vertical_angle_, camera_horizontal_angle_;
  float camera_fov_;
  float camera_speed_;
  int render_mode_;
  bool export_joint_transformations_;
  std::string joint_transformations_file_name_;
  bool use_transformations_file_;
  bool has_spline_;
  Spline spline_;
  std::vector<float> screenshot_frames_;
  std::string screenshots_folder_;
  bool has_animation_blending_;
  unsigned animation_blend_from_;
  unsigned animation_blend_to_;
  float bone_size_;
  float joint_size_;
};

#endif // Config_H_INCLUDED
