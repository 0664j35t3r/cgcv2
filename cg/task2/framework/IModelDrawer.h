/*
 * IModelDrawer.h
 *
 *  Created on: 17.02.2015
 *      Author: oberr_000
 */

#ifndef IMODELDRAWER_H_
#define IMODELDRAWER_H_

#include <GL/gl3w.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "Image.h"

class Model;
class Shader;
class Camera;
class Config;

class IModelDrawer
{
 public:
  IModelDrawer(const Model* model);
  virtual ~IModelDrawer();

  virtual void init() = 0;
  virtual void draw() = 0;
  virtual void drawJoints() = 0;
  virtual void drawBones() = 0;
  virtual void startAction(size_t action) = 0;
  virtual void update(float time) = 0;
  virtual Image makeScreenshot() = 0;
  virtual void exportJointTransformations(const std::string& filename) = 0;
  void setConfig(Config* config);
  void setShader(Shader* shader);
  void setCamera(const Camera* camera);
  void setJointSize(float size);
  void setBoneSize(float size);
  void moveTo(const glm::vec3& position);
  void orientate(const glm::quat& orientation);

 protected:
  Config* config_;
  Shader* shader_;
  const Camera* camera_;
  const Model* model_;
  float joint_size_;
  float bone_size_;
  glm::vec3 translation_;
  glm::quat orientation_;
};

#endif /* IMODELDRAWER_H_ */
