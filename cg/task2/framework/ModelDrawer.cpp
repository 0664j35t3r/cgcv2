/*
 * ModelDrawer.cpp
 *
 *  Created on: 17.02.2015
 *      Author: Peter Oberrauner
 */

#include "ModelDrawer.h"
#include "Model.h"
#include "Material.h"
#include <vector>
#include <glm/glm.hpp>
#include <GL/gl3w.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include "Shader.h"
#include "Joint.h"
#include "Animation.h"
#include "Keyframe.h"
#include "Camera.h"
#include "../task2.h"
#include "Config.h"
#include <cmath>
#include <sstream>

using std::cout;
using std::cerr;
using std::endl;

ModelDrawer::ModelDrawer(const Model* model)
    : IModelDrawer(model)
    , joints_vbo_(0)
    , action_started_(false)
    , curr_action_(0)
{
}

ModelDrawer::~ModelDrawer()
{
}

void ModelDrawer::init()
{
  cout << "Initializing the model drawer." << endl;
  for (size_t i = 0; i < model_->getMeshCount(); i++)
  {
    const Mesh& mesh = model_->getMesh(i);

    cout << "Creating vertex vbo for mesh " << i << "." << endl;
    GLBuffer* vertex_vbo = genVertexVBO(mesh);
    vertex_vbos_[i] = vertex_vbo;

    cout << "Creating normal vbo for mesh " << i << "." << endl;
    GLBuffer* normal_vbo = genNormalVBO(mesh);
    normal_vbos_[i] = normal_vbo;

    cout << "Creating triangle ibo for mesh " << i << "." << endl;
    GLBuffer* triangle_ibo = genTriangleIBO(mesh);
    triangle_ibos_[i] = triangle_ibo;

    vertices_.emplace_back(mesh.getVertexCount());
    normals_.emplace_back(mesh.getVertexCount());
  }

  cout << "Creating joints vbo." << endl;
  joints_vbo_ = genJointsVBO();

  cout << "Creating bones vbo." << endl;
  bones_vbo_ = genBonesVBO();

  cout << "Caching bone count." << endl;
  bone_count_ = calcBoneCount();

  cout << "Initializing the model matrix with the identity matrix." << endl;
  model_mat_ = glm::mat4(1);

  joint_transformations_.resize(model_->getJointCount());
}

void ModelDrawer::draw()
{
  // Pass model and normal matrix over to the shader.
  shader_->setUniformMatrix4f("model_mat", model_mat_);
  shader_->setUniformMatrix4f("normal_mat", normal_mat_);

  size_t mesh_cnt = model_->getMeshCount();
  for (size_t i = 0; i < mesh_cnt; i++)
  {
    const Mesh& mesh = model_->getMesh(i);
    const Material& material = model_->getMaterial(mesh.getMaterial());
    GLBuffer* vertex_vbo = vertex_vbos_[i];
    GLBuffer* normal_vbo = normal_vbos_[i];
    GLBuffer* triangle_ibo = triangle_ibos_[i];

    vertex_vbo->bind();

    if (action_started_)
    {
      calculateVertices(mesh.getVertices(), model_->getJoints(),
          mesh.getJoints(), mesh.getWeights(), joint_transformations_,
          vertices_[i]);
    }
    else
    {
      vertices_[i] = mesh.getVertices();
    }

    vertex_vbo->discardData();
    vertex_vbo->write(vertices_[i]);
    vertex_vbo->unbind();

    normal_vbo->bind();
    if (action_started_)
    {
      calculateNormals(vertices_[i], mesh.getTriangles(), normals_[i]);
    }
    else
    {
      normals_[i] = mesh.getNormals();
    }
    normal_vbo->discardData();
    normal_vbo->write(normals_[i]);
    normal_vbo->unbind();

    vertex_vbo->bind();
    shader_->setAttribPointer("position", GL_FLOAT, 3, 0);
    shader_->enableAttribArray("position");

    normal_vbo->bind();
    shader_->setAttribPointer("normal", GL_FLOAT, 3, 0);
    shader_->enableAttribArray("normal");

    triangle_ibo->bind();

    shader_->setUniform3f("mat_diffuse", material.getDiffuse());

    GLsizei element_cnt = static_cast<GLsizei>(mesh.getTriangleCount() * 3);
    glDrawElements(GL_TRIANGLES, element_cnt, GL_UNSIGNED_INT, 0);

    triangle_ibo->unbind();
    normal_vbo->unbind();
    vertex_vbo->unbind();
  }
}

void ModelDrawer::drawJoints()
{
  std::vector<glm::vec3> vertex_data;
  size_t joint_count = model_->getJointCount();
  for (size_t joint_index = 0; joint_index < joint_count; joint_index++)
  {
    const Joint& joint = model_->getJoint(joint_index);

    // Vertices
    const glm::mat4& joint_transform = joint_transformations_[joint_index];
    const float& js = joint_size_;
    std::vector<glm::vec3> vertices = {glm::vec3(-js, -js, js),
        glm::vec3(0, js, 0),
        glm::vec3(js, -js, js),
        glm::vec3(-js, -js, -js),
        glm::vec3(0, js, 0),
        glm::vec3(js, -js, -js),
        glm::vec3(js, -js, js),
        glm::vec3(0, js, 0),
        glm::vec3(js, -js, -js),
        glm::vec3(-js, -js, -js),
        glm::vec3(0, js, 0),
        glm::vec3(-js, -js, js),
        glm::vec3(js, -js, -js),
        glm::vec3(-js, -js, -js),
        glm::vec3(-js, -js, js),
        glm::vec3(js, -js, -js),
        glm::vec3(-js, -js, js),
        glm::vec3(js, -js, js)};

    for (size_t v_idx = 0; v_idx < vertices.size(); ++v_idx)
    {
      vertices[v_idx] =
          glm::vec3(joint_transform * glm::vec4(vertices[v_idx], 1));
    }
    std::vector<glm::ivec3> triangles = {glm::ivec3(0, 1, 2),
        glm::ivec3(5, 4, 3),
        glm::ivec3(6, 7, 8),
        glm::ivec3(9, 10, 11),
        glm::ivec3(12, 13, 14),
        glm::ivec3(15, 16, 17)};
    std::vector<glm::vec3> normals(vertices.size());
    calculateNormals(vertices, triangles, normals);

    for (size_t v_idx = 0; v_idx < vertices.size(); ++v_idx)
    {
      vertex_data.push_back(vertices[v_idx]);
      vertex_data.push_back(normals[v_idx]);
    }
  }

  joints_vbo_->bind();
  joints_vbo_->discardData();
  joints_vbo_->write(vertex_data);

  shader_->setAttribPointer("position", GL_FLOAT, 3, 24, 0);
  shader_->enableAttribArray("position");
  shader_->setAttribPointer("normal", GL_FLOAT, 3, 24, 12);
  shader_->enableAttribArray("normal");
  shader_->setUniform3f("mat_diffuse", model_->getMaterial(0).getDiffuse());
  shader_->setUniformMatrix4f("model_mat", model_mat_);
  shader_->setUniformMatrix4f("normal_mat", normal_mat_);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(joint_count * 18));

  joints_vbo_->unbind();
}

void ModelDrawer::drawBones()
{
  std::vector<glm::vec3> vertex_data;
  size_t bone_count = 0;
  for (const Joint& joint : model_->getJoints())
  {
    if (!joint.hasParent())
    {
      continue;
    }

    glm::mat4 parent_transform = joint_transformations_[joint.getParent()];

    glm::mat4 joint_transform = joint_transformations_[joint.getID()];
    glm::vec4 joint_transl = joint_transform[3];
    joint_transform = parent_transform;
    joint_transform[3] = joint_transl;

    // Vertices
    std::vector<glm::vec3> vertices = {
        glm::vec3(parent_transform * glm::vec4(bone_size_, 0, bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(-bone_size_, 0, bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(-bone_size_, 0, bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(bone_size_, 0, bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(-bone_size_, 0, bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(bone_size_, 0, bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(-bone_size_, 0, -bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(bone_size_, 0, -bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(bone_size_, 0, -bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(-bone_size_, 0, -bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(bone_size_, 0, -bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(-bone_size_, 0, -bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(bone_size_, 0, -bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(bone_size_, 0, bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(bone_size_, 0, bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(bone_size_, 0, -bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(bone_size_, 0, bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(bone_size_, 0, -bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(-bone_size_, 0, bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(-bone_size_, 0, -bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(-bone_size_, 0, -bone_size_, 1)),
        glm::vec3(parent_transform * glm::vec4(-bone_size_, 0, bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(-bone_size_, 0, -bone_size_, 1)),
        glm::vec3(joint_transform * glm::vec4(-bone_size_, 0, bone_size_, 1))};

    // Normals
    std::vector<glm::ivec3> triangles = {glm::ivec3(0, 1, 2),
        glm::ivec3(3, 4, 5),
        glm::ivec3(6, 7, 8),
        glm::ivec3(9, 10, 11),
        glm::ivec3(12, 13, 14),
        glm::ivec3(15, 16, 17),
        glm::ivec3(18, 19, 20),
        glm::ivec3(21, 22, 23)};

    std::vector<glm::vec3> normals(vertices.size());
    calculateNormals(vertices, triangles, normals);

    for (unsigned i = 0; i < vertices.size(); i++)
    {
      vertex_data.push_back(vertices[i]);
      vertex_data.push_back(normals[i]);
    }

    bone_count++;
  }

  bones_vbo_->bind();
  bones_vbo_->discardData();
  bones_vbo_->write(vertex_data);

  shader_->setAttribPointer("position", GL_FLOAT, 3, 24, 0);
  shader_->enableAttribArray("position");
  shader_->setAttribPointer("normal", GL_FLOAT, 3, 24, 12);
  shader_->enableAttribArray("normal");
  shader_->setUniform3f("mat_diffuse", model_->getMaterial(0).getDiffuse());
  shader_->setUniformMatrix4f("model_mat", model_mat_);
  shader_->setUniformMatrix4f("normal_mat", normal_mat_);
  glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(bone_count * 24));

  bones_vbo_->unbind();
}

GLBuffer* ModelDrawer::genVertexVBO(const Mesh& mesh)
{
  const std::vector<glm::vec3>& vertices = mesh.getVertices();
  const size_t byte_cnt = vertices.size() * sizeof(glm::vec3);
  cout << "Generating vertex vbo for " << vertices.size() << " vertices ("
       << byte_cnt << " bytes)." << endl;
  GLBuffer* vbo = new GLBuffer(GLBuffer::BufferType::VERTEX_BUFFER);
  vbo->create();
  vbo->bind();
  vbo->allocate(byte_cnt, GLBuffer::Usage::STREAM_DRAW);
  vbo->write(vertices);
  vbo->unbind();

  return vbo;
}

GLBuffer* ModelDrawer::genNormalVBO(const Mesh& mesh)
{
  const std::vector<glm::vec3>& normals = mesh.getNormals();
  const size_t byte_cnt = normals.size() * sizeof(glm::vec3);
  cout << "Generating normal vbo for " << normals.size() << " normals ("
       << byte_cnt << " bytes)." << endl;
  GLBuffer* vbo = new GLBuffer(GLBuffer::BufferType::VERTEX_BUFFER);
  vbo->create();
  vbo->bind();
  vbo->allocate(byte_cnt, GLBuffer::Usage::STREAM_DRAW);
  vbo->write(normals);
  vbo->unbind();

  return vbo;
}

GLBuffer* ModelDrawer::genTriangleIBO(const Mesh& mesh)
{
  const std::vector<glm::ivec3>& triangles = mesh.getTriangles();
  const size_t byte_cnt = triangles.size() * sizeof(glm::ivec3);
  cout << "Generating triangle ibo for " << triangles.size() << " triangles ("
       << byte_cnt << " bytes)." << endl;
  GLBuffer* ibo = new GLBuffer(GLBuffer::BufferType::INDEX_BUFFER);
  ibo->create();
  ibo->bind();
  ibo->allocate(byte_cnt, GLBuffer::Usage::STATIC_DRAW);
  ibo->write(triangles);
  ibo->unbind();

  return ibo;
}

GLBuffer* ModelDrawer::genJointsVBO()
{
  size_t joint_cnt = model_->getJointCount();
  const size_t byte_cnt = joint_cnt * 36 * sizeof(glm::vec3);
  cout << "Generating vbo for the joint representation (" << byte_cnt
       << " bytes)." << endl;
  GLBuffer* vbo = new GLBuffer(GLBuffer::BufferType::VERTEX_BUFFER);
  vbo->create();
  vbo->bind();
  vbo->allocate(byte_cnt, GLBuffer::Usage::STREAM_DRAW);
  vbo->unbind();
  return vbo;
}

GLBuffer* ModelDrawer::genBonesVBO()
{
  size_t bone_cnt = calcBoneCount();
  const size_t byte_cnt = bone_cnt * 48 * sizeof(glm::vec3);
  cout << "Generating vbo for the bone representation (" << byte_cnt
       << " bytes)." << endl;
  GLBuffer* vbo = new GLBuffer(GLBuffer::BufferType::VERTEX_BUFFER);
  vbo->create();
  vbo->bind();
  vbo->allocate(byte_cnt, GLBuffer::Usage::STREAM_DRAW);
  vbo->unbind();
  return vbo;
}

size_t ModelDrawer::calcBoneCount()
{
  size_t bone_cnt = model_->getJointCount();
  for (const Joint& j : model_->getJoints())
  {
    if (!j.hasParent())
    {
      bone_cnt--;
    }
  }
  return bone_cnt;
}

void ModelDrawer::startAction(size_t action)
{
  curr_action_ = action;
  action_started_ = true;
}

void ModelDrawer::update(float time)
{
  // Calculate the model matrix.
  model_mat_ = glm::translate(glm::mat4(1), translation_);
  model_mat_ *= glm::mat4_cast(orientation_);

  // Calculate the normal matrix.
  normal_mat_ = camera_->getViewMatrix() * model_mat_;
  normal_mat_ = glm::transpose(glm::inverse(normal_mat_));

  if (action_started_)
  {
    if (config_->hasAnimationBlending())
    {

      const Animation& action_1 =
          model_->getAnimation(config_->getAnimationBlendingFrom());
      const Animation& action_2 =
          model_->getAnimation(config_->getAnimationBlendingTo());

      interpolateJointsForAnimationModulation(action_1.loopTime(time), action_2.loopTime(time), model_->getJoints(), action_1.getKeyframes(), action_2.getKeyframes(), joint_transformations_);
    }
    else
    {
      const Animation& action = model_->getAnimation(curr_action_);

      if (config_->useTransformationsFile())
      {
        importJointTransformations(config_->getJointTransformationsFileName());
      }
      else
      {
        interpolateJoints(action.loopTime(time), model_->getJoints(), action.getKeyframes(), joint_transformations_);
      }
    }
  }
}

Image ModelDrawer::makeScreenshot()
{
  // Get the viewport size.
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);

  Image image(viewport[2], viewport[3], Image::Format::RGB);

  glPixelStorei(GL_PACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glReadPixels(0, 0, image.getWidth(), image.getHeight(), GL_RGB,
      GL_UNSIGNED_BYTE, image.getDataPointer());

  image.flip(true, false);

  return image;
}

void ModelDrawer::exportJointTransformations(const std::string& filename)
{
  std::ofstream file;

  file.open(filename.c_str(), std::ios::out | std::ios::binary);

  if (!file.is_open())
  {
    cerr << "Could not open output file for exporting joint transformations."
         << endl;
  }

  for (size_t joint_index = 0; joint_index < joint_transformations_.size();
       joint_index++)
  {
    int joint_id = static_cast<int>(joint_index);
    file.write(reinterpret_cast<const char*>(&joint_id), sizeof(joint_id));
    file.write(reinterpret_cast<char*>(&joint_transformations_[joint_index]),
        sizeof(glm::mat4));
  }
  file.close();
}

void ModelDrawer::importJointTransformations(const std::string& filename)
{
  std::ifstream file;

  file.open(filename.c_str(), std::ios::in | std::ios::binary);

  if (!file.is_open())
  {
    cerr << "Could not open input file for importing joint transformations."
         << endl;
  }

  while (!file.eof())
  {
    int joint_id;
    glm::mat4 transformation;

    file.read(reinterpret_cast<char*>(&joint_id), sizeof(joint_id));
    file.read(reinterpret_cast<char*>(&transformation), sizeof(glm::mat4));

    joint_transformations_[static_cast<size_t>(joint_id)] = transformation;
  }
  file.close();
}
