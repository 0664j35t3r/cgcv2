#include "Config.h"
#include <limits>
#include <iostream>
#include <sstream>
#include <glm/glm.hpp>
#include "Spline.h"

using namespace tinyxml2;

Config::Config()
{
}

Config::~Config()
{
}
const std::string& Config::getModelFileName() const
{
  return model_file_name_;
}

const std::vector<std::string>& Config::getAnimationFileNames() const
{
  return animation_file_names_;
}

const std::vector<bool>& Config::getAnimationRelativeFlags() const
{
  return animation_relative_;
}

const std::vector<float>& Config::getAnimationRepeatTime() const
{
  return animation_repeat_;
}

const glm::vec3 Config::getCameraPosition() const
{
  return camera_position_;
}

float Config::getCameraVerticalAngle() const
{
  return camera_vertical_angle_;
}

float Config::getCameraHorizontalAngle() const
{
  return camera_horizontal_angle_;
}

float Config::getCameraFOV() const
{
  return camera_fov_;
}

float Config::getCameraSpeed() const
{
  return camera_speed_;
}

int Config::getRenderMode() const
{
  return render_mode_;
}

bool Config::exportJointTransformations() const
{
  return export_joint_transformations_;
}

const std::string& Config::getJointTransformationsFileName() const
{
  return joint_transformations_file_name_;
}

bool Config::useTransformationsFile() const
{
  return use_transformations_file_;
}

bool Config::hasSpline() const
{
  return has_spline_;
}

const Spline& Config::getSpline() const
{
  return spline_;
}

bool Config::hasScreenshotFrames() const
{
  return !screenshot_frames_.empty();
}

const std::vector<float>& Config::getScreenshotFrames() const
{
  return screenshot_frames_;
}

const std::string& Config::getScreenshotsFolder() const
{
  return screenshots_folder_;
}

bool Config::hasAnimationBlending() const
{
  return has_animation_blending_;
}

unsigned Config::getAnimationBlendingFrom() const
{
  return animation_blend_from_;
}

unsigned Config::getAnimationBlendingTo() const
{
  return animation_blend_to_;
}

float Config::getBoneSize() const
{
  return bone_size_;
}

float Config::getJointSize() const
{
  return joint_size_;
}

bool Config::load(const std::string& file_name)
{
  std::cout << "Loading the config file from '" << file_name << "'."
            << std::endl;

  std::stringstream ss;

  XMLDocument doc;
  doc.LoadFile(reinterpret_cast<const char*>(file_name.c_str()));
  if (doc.ErrorID()) // ErrorId return true if file was successfully opened
  {
    std::cerr << "Config: File not found." << std::endl;
    return false;
  }

  XMLElement* model_xml = doc.FirstChildElement("model");
  if (!model_xml)
  {
    std::cerr << "Config: No model node found." << std::endl;
    return false;
  }
  model_file_name_ = model_xml->FirstChild()->Value();

  XMLElement* anim_xml = doc.FirstChildElement("animations");
  if (!anim_xml)
  {
    std::cerr << "Config: No animation node found." << std::endl;
    return false;
  }

  XMLElement* anim_file_xml = anim_xml->FirstChildElement("animation");
  while (anim_file_xml)
  {
    animation_file_names_.emplace_back(anim_file_xml->FirstChild()->Value());
    int rel = 0;
    anim_file_xml->QueryAttribute("relative", &rel);
    animation_relative_.push_back(rel != 0 ? true : false);
    float repeat = std::numeric_limits<float>::infinity();
    anim_file_xml->QueryAttribute("repeat", &repeat);
    animation_repeat_.push_back(repeat);
    anim_file_xml = anim_file_xml->NextSiblingElement("animation");
  }

  XMLElement* anim_blend_xml = anim_xml->FirstChildElement("modulate");
  if (!anim_blend_xml)
  {
    has_animation_blending_ = false;
  }
  else
  {
    has_animation_blending_ = true;
    ss.str(anim_blend_xml->FirstChild()->Value());
    ss >> animation_blend_from_ >> animation_blend_to_;
    ss.clear();
  }

  XMLElement* camera_xml = doc.FirstChildElement("camera");
  if (!camera_xml)
  {
    std::cerr << "Config: No camera node found." << std::endl;
    return false;
  }

  XMLElement* camera_pos_xml = camera_xml->FirstChildElement("position");
  if (!camera_pos_xml)
  {
    std::cerr << "Config: No camera position node found." << std::endl;
    return false;
  }
  ss.str(camera_pos_xml->FirstChild()->Value());
  ss >> camera_position_[0] >> camera_position_[1] >> camera_position_[2];
  ss.clear();

  XMLElement* camera_fov_xml = camera_xml->FirstChildElement("fov");
  if (!camera_fov_xml)
  {
    std::cerr << "Config: No camera fov node found." << std::endl;
    return false;
  }
  ss.str(camera_fov_xml->FirstChild()->Value());
  ss >> camera_fov_;
  ss.clear();

  XMLElement* camera_orientation_xml =
      camera_xml->FirstChildElement("orientation");
  if (!camera_orientation_xml)
  {
    std::cerr << "Config: No camera orientation node found." << std::endl;
    return false;
  }
  ss.str(camera_orientation_xml->FirstChild()->Value());
  ss >> camera_vertical_angle_ >> camera_horizontal_angle_;
  ss.clear();

  XMLElement* camera_speed_xml = camera_xml->FirstChildElement("speed");
  if (!camera_speed_xml)
  {
    camera_speed_ = 1.f;
  }
  else
  {
    ss.str(camera_speed_xml->FirstChild()->Value());
    ss >> camera_speed_;
    ss.clear();
  }

  XMLElement* render_mode_xml = doc.FirstChildElement("rendermode");
  if (!render_mode_xml)
  {
    std::cerr << "Config: No render mode node found." << std::endl;
    return false;
  }
  render_mode_ = std::atoi(render_mode_xml->FirstChild()->Value());

  XMLElement* export_joint_transf_xml =
      doc.FirstChildElement("export_joint_transformations");
  if (!export_joint_transf_xml)
  {
    std::cerr << "Config: No export joint transformations node found."
              << std::endl;
    return false;
  }
  const char* export_joint_transf_val =
      export_joint_transf_xml->FirstChild()->Value();
  export_joint_transformations_ =
      (std::strcmp("1", export_joint_transf_val) == 0);

  XMLElement* joint_trans_file_xml =
      doc.FirstChildElement("joint_transformations_file");
  if (!joint_trans_file_xml)
  {
    std::cerr << "Config: No joint transformations file node found."
              << std::endl;
    return false;
  }
  joint_transformations_file_name_ =
      joint_trans_file_xml->FirstChild()->Value();

  XMLElement* use_trans_file_xml =
      doc.FirstChildElement("use_transformations_file");
  if (!use_trans_file_xml)
  {
    std::cerr << "Config: No use transformations file node found." << std::endl;
    return false;
  }
  const char* use_joint_transf_val = use_trans_file_xml->FirstChild()->Value();
  use_transformations_file_ = (std::strcmp("1", use_joint_transf_val) == 0);

  XMLElement* spline_xml = doc.FirstChildElement("movement_spline");
  if (!spline_xml)
  {
    std::cerr << "Config: No spline node found." << std::endl;
    has_spline_ = false;
  }
  else
  {
    has_spline_ = true;

    spline_ = Spline();
    XMLElement* point_xml = spline_xml->FirstChildElement("point");
    while (point_xml)
    {
      ss.str(point_xml->FirstChild()->Value());

      float t;
      glm::vec3 p;
      ss >> t >> p[0] >> p[1] >> p[2];
      ss.clear();

      spline_.addPoint(t, p);

      point_xml = point_xml->NextSiblingElement();
    }
    spline_.calculateAndStoreTangents();
  }

  XMLElement* screenshots_xml = doc.FirstChildElement("screenshots");
  if (!screenshots_xml)
  {
    std::cerr << "Config: No screenshots node found." << std::endl;
  }
  else
  {
    ss.str(screenshots_xml->FirstChild()->Value());
    while (ss)
    {
      float frame_time;
      ss >> frame_time;
      screenshot_frames_.push_back(frame_time);
    }
    ss.clear();
  }

  XMLElement* screenshots_folder_xml =
      doc.FirstChildElement("screenshots_folder");
  if (!screenshots_folder_xml)
  {
    std::cerr << "Config: No screenshots folder node found." << std::endl;
    screenshots_folder_ = "output/screenshots";
  }
  else
  { 
    ss.str(screenshots_folder_xml->FirstChild()->Value());
    ss >> screenshots_folder_;
    ss.clear();
  }

  XMLElement* bone_size_xml = doc.FirstChildElement("bone_size");
  if (!bone_size_xml)
  {
    bone_size_ = 0.03f;
  }
  else
  {
    ss.str(bone_size_xml->FirstChild()->Value());
    ss >> bone_size_;
    ss.clear();
  }

  XMLElement* joint_size_xml = doc.FirstChildElement("joint_size");
  if (!joint_size_xml)
  {
    joint_size_ = 0.15f;
  }
  else
  {
    ss.str(joint_size_xml->FirstChild()->Value());
    ss >> joint_size_;
    ss.clear();
  }

  return true;
}
