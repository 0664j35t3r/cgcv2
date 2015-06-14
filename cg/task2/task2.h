// ============================================================================
//
//       Filename:  task2.h
//
//    Description:  Tasks implemented by students.
//
//        Created:  02/04/15 15:09:22
//
//         Author:  Peter Oberrauner
//
// ============================================================================

#include <vector>

#include <glm/glm.hpp>

#include "Joint.h"
#include "Keyframe.h"
#include "Spline.h"



void interpolateJoints(float time,
  const std::vector<Joint>& joints,
  const std::vector<Keyframe>& keyframes,
  std::vector<glm::mat4>& joint_transformations);

void calculateVertices(const std::vector<glm::vec3>& bindpose_vertices,
    const std::vector<Joint>& joints,
    const std::vector<std::vector<size_t>>& vertex_joints,
    const std::vector<std::vector<float>>& vertex_weights,
    const std::vector<glm::mat4>& joint_transformations,
    std::vector<glm::vec3>& animated_vertices);

void calculateNormals(const std::vector<glm::vec3>& vertices,
    const std::vector<glm::ivec3>& triangles,
    std::vector<glm::vec3>& out_normals);

void interpolateJointsForAnimationModulation(float time_1,
  float time_2,
  const std::vector<Joint>& joints,
  const std::vector<Keyframe>& keyframes_1,
  const std::vector<Keyframe>& keyframes_2,
  std::vector<glm::mat4>& joint_transformations);

void interpolateSpline(float time,
  const SplinePoint* points,
  size_t num_points,
  glm::vec3& position,
  glm::vec3& tangent);
