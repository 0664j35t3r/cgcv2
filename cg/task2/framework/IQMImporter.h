/*
 * IQMImporter.h
 *
 *  Created on: 16.02.2015
 *      Author: oberr_000
 */

#ifndef IQMIMPORTER_H_
#define IQMIMPORTER_H_

#include <limits>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <map>

class InFile;
class Model;
class Mesh;
class Joint;

class IQMImporter
{
public:
  IQMImporter();
  virtual ~IQMImporter();

  Model* loadModel(const std::string& path, const std::vector<std::string>& animation_files, const std::vector<float>& animation_repeat_time, const std::vector<bool>& make_relative);

private:
  InFile* file_;
  Model* model_;
  Mesh* mesh_;
  std::vector<std::string> texts_; 
  std::vector<glm::vec3> position_array_;
  std::vector<glm::vec3> normal_array_;
  std::vector<std::vector<unsigned> > joint_index_array_;
  std::vector<std::vector<float> > joint_weight_array_;
  std::vector<glm::ivec3> triangles_;
  std::map<unsigned, unsigned> joint_index_map_;

  enum VertexArrayType
  {
    IQM_POSITION = 0,
    IQM_TEXCOORD = 1,
    IQM_NORMAL = 2,
    IQM_TANGENT = 3,
    IQM_BLENDINDEXES = 4,
    IQM_BLENDWEIGHTS = 5,
    IQM_COLOR = 6
  };

  enum VertexArrayFormat
  {
    IQM_BYTE = 0,
    IQM_UBYTE = 1,
    IQM_SHORT = 2,
    IQM_USHORT = 3,
    IQM_INT = 4,
    IQM_UINT = 5,
    IQM_HALF = 6,
    IQM_FLOAT = 7,
    IQM_DOUBLE = 8
  };

  class IQMPose
  {
  public:
    int parent;
    unsigned int mask;
    std::vector<float> offset, scale;
  };

  class IQMAnim
  {
  public:
    unsigned int name, frame_start, frame_end;
    float framerate;
    unsigned int flags;
  };

  const static size_t HEADER_SIZE;

  void loadVertexArrays();
  void loadTriangles();
  void loadPositionArray(unsigned int vertex_cnt, unsigned int format);
  void loadNormalArray(unsigned int vertex_cnt, unsigned int format);
  void loadJointIndexArray(unsigned int vertex_cnt, unsigned int format);
  void loadJointWeightArray(unsigned int vertex_cnt, unsigned int format);
  void loadMeshes();
  void loadTexts();
  void loadJoints();
  std::vector<Joint> sortJoints(std::vector<Joint>& joints);
  void sortJointsRecursive(std::vector<Joint>& joints, int curr_joint_idx, std::vector<Joint>& out_joints);
  void fixJointIDs(std::vector<Joint>& joints);
  void loadAnimations(InFile& animation_file, float repeat_time = std::numeric_limits<float>::quiet_NaN(), bool make_relative = false);

};

#endif /* IQMIMPORTER_H_ */
