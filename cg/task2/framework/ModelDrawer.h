/*
 * ModelDrawer.h
 *
 *  Created on: 17.02.2015
 *      Author: oberr_000
 */

#ifndef MODELDRAWER_H_
#define MODELDRAWER_H_

#include <map>
#include <fstream>

#include <string>
#include "IModelDrawer.h"
#include "GLBuffer.h"
#include "Mesh.h"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class Model;
class Animation;
class Keyframe;

class ModelDrawer : public IModelDrawer
{
public:
  ModelDrawer(const Model* model);
  virtual ~ModelDrawer();

  void init();
  void draw();
  void drawJoints();
  void drawBones();
  void startAction(size_t action);
  void update(float time);
  Image makeScreenshot();

  void importJointTransformations(const std::string& filename);

private:
  std::map<size_t, GLBuffer*> vertex_vbos_;
  std::map<size_t, GLBuffer*> normal_vbos_;
  std::map<size_t, GLBuffer*> triangle_ibos_;
  GLBuffer* joints_vbo_;
  GLBuffer* bones_vbo_;
  glm::mat4 model_mat_;
  glm::mat4 normal_mat_;

  std::vector<glm::mat4> joint_transformations_;
  std::vector<glm::mat4> joint_translations_;
  size_t bone_count_;

  std::vector<std::vector<glm::vec3>> vertices_;
  std::vector<std::vector<glm::vec3>> normals_;

  bool action_started_;
  size_t curr_action_;

  GLBuffer* genVertexVBO(const Mesh& mesh);
  GLBuffer* genNormalVBO(const Mesh& mesh);
  GLBuffer* genTriangleIBO(const Mesh& mesh);
  GLBuffer* genJointsVBO();
  GLBuffer* genBonesVBO();
  size_t calcBoneCount();

  void exportJointTransformations(const std::string& filename);
};

#endif /* MODELDRAWER_H_ */
