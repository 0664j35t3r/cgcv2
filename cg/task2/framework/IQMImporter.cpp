/*
 * IQMImporter.cpp
 *
 *  Created on: 16.02.2015
 *      Author: oberr_000
 */

#include "IQMImporter.h"
#include "Model.h"
#include "InFile.h"
#include "Mesh.h"
#include "Material.h"
#include "Joint.h"
#include "Keyframe.h"
#include "Animation.h"
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using std::string;
using std::vector;
using std::cout;
using std::cerr;
using std::endl;

const size_t IQMImporter::HEADER_SIZE = 120;

IQMImporter::IQMImporter()
    : file_(0)
    , model_(0)
    , mesh_(0)
{
}

IQMImporter::~IQMImporter()
{
}

Model* IQMImporter::loadModel(const std::string& path,
    const std::vector<std::string>& animation_files,
    const std::vector<float>& animation_repeat_time,
    const std::vector<bool>& make_relative)
{
  file_ = new InFile(path, InFile::OpenMode::BINARY);

  cout << "Caching model file." << endl;
  file_->cache();

  model_ = new Model();

  file_->position(16);
  unsigned int version = file_->getUnsignedInt();
  unsigned int file_size = file_->getUnsignedInt();

  std::cout << "IQM Version: " << version << std::endl;
  std::cout << "File size: " << file_size << std::endl;

  cout << "Creating default material." << endl;
  Material material;
  material.setAlpha(1.f);
  material.setDiffuse(glm::vec3(0.7f, 0.7f, 0.7f));

  cout << "Adding default material to model." << endl;
  model_->addMaterial(material);

  loadTexts();
  loadVertexArrays();
  loadTriangles();
  loadJoints();
  loadMeshes();

  auto reapeat_it = animation_repeat_time.begin();
  auto make_relative_it = make_relative.begin();
  for (auto anim_it = animation_files.begin(); anim_it != animation_files.end();
    ++anim_it, ++make_relative_it, ++reapeat_it)
  {
    InFile anim_file(*anim_it, InFile::OpenMode::BINARY);
    anim_file.cache();
    loadAnimations(anim_file, *reapeat_it, *make_relative_it);
  }

  return model_;
}

void IQMImporter::loadMeshes()
{
  cout << "Loading meshes from IQM file." << endl;

  file_->position(36);
  unsigned int mesh_cnt = file_->getUnsignedInt();
  unsigned int mesh_ofs = file_->getUnsignedInt();

  cout << "The IQM file contains " << mesh_cnt << " meshes." << endl;

  if (mesh_cnt == 0)
    return;

  file_->position(mesh_ofs);

  for (unsigned int mesh_idx = 0; mesh_idx < mesh_cnt; mesh_idx++)
  {
    cout << "Loading mesh " << mesh_idx << endl;

    Mesh mesh;

    unsigned int name = file_->getUnsignedInt();

    unsigned int material = file_->getUnsignedInt();
    cout << "Setting material of mesh to the default material." << endl;
    mesh.setMaterial(0);

    unsigned int vertex_begin = file_->getUnsignedInt();
    unsigned int vertex_cnt = file_->getUnsignedInt();
    cout << "This mesh contains " << vertex_cnt << " vertices." << endl;

    mesh.allocateSpace(vertex_cnt);

    for (unsigned int vertex_idx = vertex_begin;
         vertex_idx < vertex_begin + vertex_cnt; vertex_idx++)
    {
      mesh.addVertex(position_array_[vertex_idx]);
      mesh.addNormal(normal_array_[vertex_idx]);
      std::vector<float> weights = joint_weight_array_[vertex_idx];
      std::vector<unsigned> joint_indices = joint_index_array_[vertex_idx];

      for (unsigned i = 0; i < 4; i++)
      {
        if (weights[i] != 0.f)
        {
          mesh.addWeight(vertex_idx - vertex_begin, weights[i]);
          mesh.addJoint(
              vertex_idx - vertex_begin, joint_index_map_[joint_indices[i]]);
        }
      }
    }

    unsigned int triangle_begin = file_->getUnsignedInt();
    unsigned int triangle_cnt = file_->getUnsignedInt();
    cout << "This mesh contains " << triangle_cnt << " triangles." << endl;
    for (unsigned int i = triangle_begin; i < triangle_begin + triangle_cnt;
         i++)
      mesh.addTriangle(triangles_[i]);

    cout << "Adding the mesh to the model." << endl;
    model_->addMesh(mesh);
  }
}

void IQMImporter::loadVertexArrays()
{
  cout << "Loading vertex arrays from IQM file." << endl;

  file_->position(44);
  unsigned int va_cnt = file_->getUnsignedInt();
  unsigned int vertex_cnt = file_->getUnsignedInt();
  unsigned int va_ofs = file_->getUnsignedInt();

  cout << "The IQM file contains " << vertex_cnt << " vertices." << endl;

  if (va_cnt == 0 || va_ofs == 0)
    return;

  file_->position(va_ofs);
  for (unsigned int va_idx = 0; va_idx < va_cnt; va_idx++)
  {
    unsigned int type = file_->getUnsignedInt();
    unsigned int flags = file_->getUnsignedInt();
    unsigned int format = file_->getUnsignedInt();
    unsigned int size = file_->getUnsignedInt();
    unsigned int ofs = file_->getUnsignedInt();
    size_t return_pos = file_->getPosition();

    file_->position(ofs);

    if (type == VertexArrayType::IQM_POSITION)
      loadPositionArray(vertex_cnt, format);
    else if (type == VertexArrayType::IQM_NORMAL)
      loadNormalArray(vertex_cnt, format);
    else if (type == VertexArrayType::IQM_BLENDINDEXES)
      loadJointIndexArray(vertex_cnt, format);
    else if (type == VertexArrayType::IQM_BLENDWEIGHTS)
      loadJointWeightArray(vertex_cnt, format);

    file_->position(return_pos);
  }
}

void IQMImporter::loadPositionArray(unsigned int vertex_cnt,
    unsigned int format)
{
  cout << "Loading position vertex array from IQM file." << endl;

  if (format != VertexArrayFormat::IQM_FLOAT)
  {
    cerr << "Vertex data in unsupported format." << endl;
    return;
  }

  for (unsigned int i = 0; i < vertex_cnt; i++)
  {
    std::vector<float> v = file_->getFloatVector(3);
    glm::vec3 pos = glm::make_vec3(&v[0]);
    position_array_.push_back(pos);
  }
}

void IQMImporter::loadNormalArray(unsigned int vertex_cnt, unsigned int format)
{
  cout << "Loading normal vertex array from IQM file." << endl;
  if (format != VertexArrayFormat::IQM_FLOAT)
  {
    cerr << "Normal data in unsupported format." << endl;
    return;
  }

  for (unsigned int i = 0; i < vertex_cnt; i++)
  {
    std::vector<float> n = file_->getFloatVector(3);
    glm::vec3 normal = glm::make_vec3(&n[0]);
    normal_array_.push_back(normal);
  }
}

void IQMImporter::loadJointIndexArray(unsigned int vertex_cnt,
    unsigned int format)
{
  cout << "Loading joint index vertex array from IQM file." << endl;

  if (format != VertexArrayFormat::IQM_UBYTE &&
      format != VertexArrayFormat::IQM_INT)
  {
    cerr << "Joint index data in unsupported format." << endl;
    return;
  }

  for (unsigned int i = 0; i < vertex_cnt; i++)
  {
    std::vector<unsigned> indices;
    if (format == VertexArrayFormat::IQM_UBYTE)
      for (unsigned i = 0; i < 4; i++)
        indices.push_back(file_->getUnsignedChar());
    else if (format == VertexArrayFormat::IQM_INT)
      indices = file_->getUnsignedIntVector(4);
    joint_index_array_.push_back(indices);
  }
}

void IQMImporter::loadJointWeightArray(unsigned int vertex_cnt,
    unsigned int format)
{
  cout << "Loading joint weight vertex array from IQM file." << endl;

  if (format != VertexArrayFormat::IQM_FLOAT &&
      format != VertexArrayFormat::IQM_UBYTE)
  {
    cerr << "Joint weight data in unsupported format." << endl;
    return;
  }

  for (unsigned int i = 0; i < vertex_cnt; i++)
  {
    std::vector<float> weights;
    if (format == VertexArrayFormat::IQM_FLOAT)
      weights = file_->getFloatVector(4);
    else if (format == VertexArrayFormat::IQM_UBYTE)
      for (unsigned i = 0; i < 4; i++)
        weights.push_back(file_->getUnsignedChar() / 255.f);
    joint_weight_array_.push_back(weights);
  }
}

void IQMImporter::loadTriangles()
{
  cout << "Loading triangles from IQM file." << endl;
  file_->position(56);
  unsigned int triangle_cnt = file_->getUnsignedInt();
  unsigned int triangle_ofs = file_->getUnsignedInt();

  cout << "The IQM file contains " << triangle_cnt << " triangles." << endl;

  if (triangle_cnt == 0 || triangle_ofs == 0)
    return;

  file_->position(triangle_ofs);
  for (unsigned int triangle_idx = 0; triangle_idx < triangle_cnt;
       triangle_idx++)
  {
    std::vector<unsigned int> tmp = file_->getUnsignedIntVector(3);
    glm::ivec3 tri = glm::make_vec3(&tmp[0]);
    triangles_.push_back(tri);
  }
}

void IQMImporter::loadTexts()
{
  cout << "Loading texts from IQM file." << endl;

  file_->position(28);
  unsigned int text_cnt = file_->getUnsignedInt();
  unsigned int text_ofs = file_->getUnsignedInt();

  if (text_cnt == 0 || text_ofs == 0)
    return;

  file_->position(text_ofs);

  size_t char_cnt = 0;
  while (char_cnt < text_cnt)
  {
    std::string text = file_->getString();
    char_cnt += text.size() + 1;
    texts_.push_back(text);
  }

  cout << "The IQM file contains " << texts_.size() << " texts." << endl;
}

void IQMImporter::loadJoints()
{
  cout << "Loading joints from IQM file." << endl;
  file_->position(68);

  unsigned int joint_cnt = file_->getUnsignedInt();
  unsigned int joint_ofs = file_->getUnsignedInt();

  cout << "The IQM file contains " << joint_cnt << " joints." << endl;

  std::vector<Joint> joints;
  file_->position(joint_ofs);
  for (unsigned int i = 0; i < joint_cnt; i++)
  {
    unsigned int name = file_->getUnsignedInt();
    int parent = file_->getUnsignedInt();
    std::vector<float> translation_data = file_->getFloatVector(3);
    glm::vec3 translation = glm::make_vec3(&translation_data[0]);
    std::vector<float> rotation_data = file_->getFloatVector(4);
    glm::quat rotation = glm::make_quat(&rotation_data[0]);
    std::vector<float> scale_data = file_->getFloatVector(3);
    glm::vec3 scale = glm::make_vec3(&scale_data[0]);

    Joint j;
    j.setID(i);
    j.setOffset(translation);
    j.setRotation(glm::normalize(rotation));
    j.setParent(parent);

    glm::mat4 base_mat = glm::translate(glm::mat4(1), j.getOffset());
    base_mat *= glm::mat4_cast(j.getRotation());
    j.setBasePoseMatrix(base_mat);

    joints.push_back(j);
  }

  joints = sortJoints(joints);
  fixJointIDs(joints);

  for (Joint& j : joints)
  {
    int p_idx = j.getParent();
    if (p_idx >= 0)
    {
      Joint& p = joints[p_idx];
      glm::mat4 base_mat = p.getBasePoseMatrix() * j.getBasePoseMatrix();
      j.setBasePoseMatrix(base_mat);
    }
    model_->addJoint(j);
  }
}

std::vector<Joint> IQMImporter::sortJoints(std::vector<Joint>& joints)
{
  std::vector<Joint> sorted;
  for (int i = 0; i < joints.size(); i++)
    sortJointsRecursive(joints, i, sorted);
  return sorted;
}

void IQMImporter::sortJointsRecursive(std::vector<Joint>& joints,
    int curr_joint_idx,
    std::vector<Joint>& out_joints)
{
  Joint curr_joint = joints[curr_joint_idx];
  for (int out_idx = 0; out_idx < out_joints.size(); out_idx++)
  {
    if (curr_joint.getID() == out_joints[out_idx].getID())
    {
      return;
    }
  }

  if (curr_joint.getParent() < 0)
  {
    out_joints.push_back(curr_joint);
    return;
  }

  sortJointsRecursive(joints, curr_joint.getParent(), out_joints);
  out_joints.push_back(curr_joint);
}

void IQMImporter::fixJointIDs(std::vector<Joint>& joints)
{
  for (int j_idx = 0; j_idx < joints.size(); j_idx++)
  {
    Joint& j = joints[j_idx];
    joint_index_map_[j.getID()] = j_idx;
  }

  for (int j_idx = 0; j_idx < joints.size(); j_idx++)
  {
    Joint& j = joints[j_idx];
    j.setID(joint_index_map_[j.getID()]);
    if (j.getParent() >= 0)
      j.setParent(joint_index_map_[j.getParent()]);
  }
}

void IQMImporter::loadAnimations(InFile& animation_file, float repeat_time, bool make_relative)
{
  animation_file.position(76);

  unsigned int pose_cnt = animation_file.getUnsignedInt();
  unsigned int pose_ofs = animation_file.getUnsignedInt();

  std::vector<IQMPose> poses;
  animation_file.position(pose_ofs);
  for (unsigned i = 0; i < pose_cnt; i++)
  {
    IQMPose pose;
    pose.parent = animation_file.getUnsignedInt();
    pose.mask = animation_file.getUnsignedInt();
    pose.offset = animation_file.getFloatVector(10);
    pose.scale = animation_file.getFloatVector(10);
    poses.push_back(pose);
  }

  animation_file.position(84);

  unsigned int anim_cnt = animation_file.getUnsignedInt();
  unsigned int anim_ofs = animation_file.getUnsignedInt();

  cout << "Animation Count: " << anim_cnt << endl;

  std::vector<IQMAnim> anims;
  animation_file.position(anim_ofs);
  for (unsigned i = 0; i < anim_cnt; i++)
  {
    IQMAnim anim;
    anim.name = animation_file.getUnsignedInt();
    anim.frame_start = animation_file.getUnsignedInt();
    anim.frame_end = anim.frame_start + animation_file.getUnsignedInt();
    anim.framerate = animation_file.getFloat();
    anim.flags = animation_file.getUnsignedInt();
    anims.push_back(anim);
  }

  animation_file.position(92);

  unsigned int frame_cnt = animation_file.getUnsignedInt();
  unsigned int frame_channel_cnt = animation_file.getUnsignedInt();
  unsigned int frame_ofs = animation_file.getUnsignedInt();

  animation_file.position(frame_ofs);
  std::vector<unsigned short> frames =
      animation_file.getUnsignedShortVector(frame_cnt * frame_channel_cnt);

  std::vector<Keyframe> keyframes(frame_cnt);
  unsigned frame_idx = 0;
  for (unsigned frame = 0; frame < frame_cnt; frame++)
  {
    for (unsigned j = 0; j < pose_cnt; j++)
    {
      IQMPose& pose = poses[j];

      float transforms[10];
      for (unsigned i = 0; i < 10; i++)
      {
        transforms[i] = pose.offset[i];
        if (pose.mask & (1 << i))
          transforms[i] += frames[frame_idx++] * pose.scale[i];
      }
      glm::vec3 translate(transforms[0], transforms[1], transforms[2]);
      glm::quat rotate(
          transforms[6], transforms[3], transforms[4], transforms[5]);
      glm::vec3 scale(transforms[7], transforms[8], transforms[9]);

      keyframes[frame].setTranslation(joint_index_map_[j], translate);
      keyframes[frame].setRotation(joint_index_map_[j], rotate);
    }
  }

  Keyframe base_frame = keyframes[0];
  for (IQMAnim& anim : anims)
  {
    Animation animation;
    animation.setRepeatTime(repeat_time);
    for (unsigned i = anim.frame_start; i < anim.frame_end; i++)
    {
      if (make_relative)
      {
        keyframes[i] = keyframes[i].relativeTo(base_frame);
      }
      keyframes[i].setTime((i - anim.frame_start) / anim.framerate);
    }
    animation.setKeyframes(&keyframes[0] + anim.frame_start, anim.frame_end - anim.frame_start);
    model_->addAnimation(animation);
  }
}
