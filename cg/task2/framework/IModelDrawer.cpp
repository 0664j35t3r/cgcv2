/*
 * IModelDrawer.cpp
 *
 *  Created on: 17.02.2015
 *      Author: oberr_000
 */

#include "IModelDrawer.h"
#include "Model.h"
#include <GL/gl3w.h>
#include "Shader.h"
#include "Camera.h"
#include "Config.h"

IModelDrawer::IModelDrawer(const Model* model)
    : config_(0)
    , shader_(0)
    , camera_(0)
    , model_(model)
    , joint_size_(0.1f)
    , bone_size_(0.01f)
    , translation_(0.f)
{
}

IModelDrawer::~IModelDrawer()
{
}

void IModelDrawer::setConfig(Config* config)
{
  config_ = config;
}

void IModelDrawer::setShader(Shader* shader)
{
  shader_ = shader;
}

void IModelDrawer::setCamera(const Camera* camera)
{
  camera_ = camera;
}

void IModelDrawer::setJointSize(float size)
{
  joint_size_ = size;
}

void IModelDrawer::setBoneSize(float size)
{
  bone_size_ = size;
}

void IModelDrawer::moveTo(const glm::vec3& position)
{
  translation_ = position;
}

void IModelDrawer::orientate(const glm::quat& orientation)
{
  orientation_ = orientation;
}
