// ============================================================================
//       Filename:  task2.cpp
//
//    Description:  Tasks implemented by students.
//
//        Created:  02/04/15 15:32:02
//
//         Author:  Peter Oberrauner
// ============================================================================

#include <iostream>

#include <algorithm>
#include <iterator>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "task2.h"


using std::endl;
using std::cout;
using namespace glm;


void interpolateJoints(float time,
  const std::vector<Joint>& joints,
  const std::vector<Keyframe>& keyframes,
  std::vector<glm::mat4>& joint_transformations)
{
  //  TODO:
  //  For a given time compute the articulated
  //  pose of the skeleton by interpolating between the two
  //  enclosing keyframes. Store the articulated skeleton in
  //  the joint_transformations array, as matrices that transform
  //  from the local joint coordinate system into the object 
  //  coordinate system.

  //  Note that the first joint in the joints array is the root joint
  //  and you can access the parent of each joint using the index

  //  provided by the Joint::getParent method. Also note that the
  //  joints array is sorted in such a way, that all children of a
  //  joint have greater indices than the joint itself.

  //  The rotation and translation of a joint can be accessed using
  //  the Keyframe class: Keyframe::getTranslation(joint) and
  //  Keyframe::getRotation(joint)

  //  Further useful functions are glm::slerp to perform a spherical
  //  linear interpolation of two quaternions, glm::translate to
  //  construct a translation matrix from a vector, and glm::mat4_cast
  //  to cast a quaternion into a transformation matrix.

  //  Keyframe::getTranslation(joint)
  //  Keyframe::getRotation(joint)
  //  glm::slerp
  //  glm::translate
  //  glm::mat4_cast

  glm::mat4 relative_trans;
  glm::mat4 relative_rotat;

  // interpolate between those
  Keyframe kf1;
  Keyframe kf2;

  for (int i = 0; i < keyframes.size() - 1; i++)
  {
    if(keyframes[i].getTime() <= time && time <= keyframes[i + 1].getTime())
    {
      kf1 = keyframes[i];
      kf2 = keyframes[i + 1];
    }
  }

  float ratio_time = ((time - kf1.getTime()) / (kf2.getTime() - kf1.getTime()));

  // iterate over sorted tree structure, define pose position
  for (int i = 0; i < joints.size(); i++)
  {
    vec3 t1 = kf1.getTranslation(i);
    // vec3 t2 = kf2.getTranslation(joints[i].getID());
    vec3 t2 = kf2.getTranslation(i);

    quat kf1_r1 = kf1.getRotation(i);
    quat kf2_r2 = kf2.getRotation(i);

    vec3 translation_interpolation = t1 * (1 - ratio_time) + t2 * ratio_time;
    mat4 rotation_interpolation = mat4_cast(slerp(kf1_r1, kf2_r2, ratio_time));

    mat4 identity_matrix(1); // fill diagonal 1;
    mat4 translattion_m = translate(identity_matrix, translation_interpolation);
    mat4 transformation = translattion_m * rotation_interpolation;

    if(joints[i].getParent() < 0)
      joint_transformations[i] = transformation;//translattion_m * rotation_interpolation;
    else
      joint_transformations[i] = joint_transformations[joints[i].getParent()] * transformation;
  }
}

void calculateVertices(const std::vector<glm::vec3>& bindpose_vertices,
    const std::vector<Joint>& joints,
    const std::vector<std::vector<size_t>>& vertex_joints,
    const std::vector<std::vector<float>>& vertex_weights,
    const std::vector<glm::mat4>& joint_transformations,
    std::vector<glm::vec3>& animated_vertices)
{
  //  TODO:
  //  - Given the bindpose vertices and the articulated skeleton,
  //    compute the animated vertices attached to the skeleton.
  //
  //    The bindpose vertices are given as bindpose_vertices. For every vertex, the
  //    joints this vertex is bound to are given in the vertex_joints
  //    array. You can simply run through all vertices
  //    and index the vertex_joints array with that index. For
  //    every joint the vertex is bound to, the resulting array
  //    holds the actual id of the joint. The joints array
  //    can then be accessed with that index.
  //    The vertex_weights array holds the weights describing the
  //    amount a vertex is bound to the respective joint. You can
  //    access the vertex_weights array the same way as the 
  //    vertex_joints array.
  //    The inverse bindpose matrix for a joint is provided by the
  //    joint member function Joint::getInverseBindPoseMatrix().
  //    Store the transformed vertices in animated_vertices.
  //

  for (int vj_iter = 0; vj_iter < vertex_joints.size(); vj_iter++)
  {
    std::vector<size_t> v_joints = vertex_joints[vj_iter];
    std::vector<float> v_weight = vertex_weights[vj_iter];

    vec4 homogen_transformation(bindpose_vertices[vj_iter], 1); //(x,y,z,1)

    vec4 tmp(0);
    for (int v_iter = 0; v_iter < v_joints.size(); v_iter++)
      tmp += joint_transformations[v_joints[v_iter]] * joints[v_joints[v_iter]].getInverseBindPoseMatrix() * homogen_transformation * v_weight[v_iter]; // s7: J J^-1 x * weight

    animated_vertices[vj_iter] = vec3(tmp[0], tmp[1], tmp[2]); // save vec vertices
  }
}

void calculateNormals(const std::vector<glm::vec3>& vertices,
    const std::vector<glm::ivec3>& triangles,
    std::vector<glm::vec3>& normals)
{
  //  TODO:
  //  Given a number of triangles (triangles) as indices into
  //  the vertices (vertices) array, compute the normals for all
  //  vertices under the assumption that this is a smooth mesh.
  //  Store the result in normals
  //
  for (int triangle_iter = 0; triangle_iter < triangles.size(); triangle_iter++)
  {
    //                 triangle i mit ecke
    vec3 v1 = vertices[triangles[triangle_iter][0]];
    vec3 v2 = vertices[triangles[triangle_iter][1]];
    vec3 v3 = vertices[triangles[triangle_iter][2]];

    vec3 normal_v_triangle = normalize(cross(v2 - v1, v2 - v3));

    normals[triangles[triangle_iter][0]] += normal_v_triangle;
    normals[triangles[triangle_iter][1]] += normal_v_triangle;
    normals[triangles[triangle_iter][2]] += normal_v_triangle;
  }
}

void interpolateJointsForAnimationModulation(float time_1,
  float time_2,
  const std::vector<Joint>& joints,
  const std::vector<Keyframe>& keyframes_1,
  const std::vector<Keyframe>& keyframes_2,
  std::vector<glm::mat4>& joint_transformations)
{
  //  TODO:
  //  For a main animation (keyframes_1) and a modulation (keyframes_2),
  //  computation the modulation on top of the main animation. At 
  //  the same time perform the interpolation between keyframes of
  //  both actions. This method requires the same steps as interpolateJoints(),
  //  while you have to additionally apply the modulation for each
  //  joint.
  //
  //  Note that you have to apply the modulation on the parent before
  //  computing the transformation for its children.
  //

  glm::mat4 relative_trans;
  glm::mat4 relative_rotat;

  // interpolate between those
  Keyframe kf1;
  Keyframe kf2;

  for (int i = 0; i < keyframes_1.size() - 1; i++)
  {
    if(keyframes_1[i].getTime() <= time_1 && time_1 <= keyframes_1[i + 1].getTime())
    {
      kf1 = keyframes_1[i];
      kf2 = keyframes_1[i + 1];
    }
  }

  float ratio_time = ((time_1 - kf1.getTime()) / (kf2.getTime() - kf1.getTime()));

  // 2 scenario

  Keyframe kf1_2;
  Keyframe kf2_2;

  for (int i = 0; i < keyframes_2.size() - 1; i++)
  {
    if(keyframes_2[i].getTime() <= time_2 && time_2 <= keyframes_2[i + 1].getTime())
    {
      kf1_2 = keyframes_2[i];
      kf2_2 = keyframes_2[i + 1];
    }
  }

  float ratio_time_2 = ((time_2 - kf1_2.getTime()) / (kf2_2.getTime() - kf1_2.getTime()));

  // iterate over sorted tree structure, define pose position
  for (int i = 0; i < joints.size(); i++)
  {
    // scenario 1
    vec3 t1 = kf1.getTranslation(i);
    vec3 t2 = kf2.getTranslation(i);

    quat kf1_r1 = kf1.getRotation(i);
    quat kf2_r2 = kf2.getRotation(i);


    // scenario 2
    vec3 t1_2 = kf1_2.getTranslation(i);
    vec3 t2_2 = kf2_2.getTranslation(i);

    quat kf1_r1_2 = kf1_2.getRotation(i);
    quat kf2_r2_2 = kf2_2.getRotation(i);

    vec3 translation_interpolation = t1_2 * (1 - ratio_time_2) + t2_2 * ratio_time_2 + t1 * (1 - ratio_time) + t2 * ratio_time;
    mat4 rotation_interpolation = mat4_cast(slerp(kf1_r1, kf2_r2, ratio_time));
    mat4 rotation_interpolation_2 = mat4_cast(slerp(kf1_r1_2, kf2_r2_2, ratio_time_2));

    mat4 identity_matrix(1); // fill diagonal 1;
    mat4 translattion_m = translate(identity_matrix, translation_interpolation);
    mat4 transformation = translattion_m * rotation_interpolation * rotation_interpolation_2;

    if (joints[i].getParent() < 0)
      joint_transformations[i] = transformation; //translattion_m * rotation_interpolation;
    else
      joint_transformations[i] =
              joint_transformations[joints[i].getParent()] * transformation;
  }
}

void interpolateSpline(float time,
  const SplinePoint* points,
  size_t num_points,
  glm::vec3& position,
  glm::vec3& tangent)
{
  //  TODO:
  //  Compute the position and tangent direction of a cubic hermite spline for a 
  //  certain point in time. The points array holds the control 
  //  points of the spline, which provide the time (getTime()) of
  //  the control point, its position (getPoint()) and its tangent
  //  (getTangent()).
  //  In case the requested time is before the first point or 
  //  after the last point, return the first or last point, respectively. 
  //
  const SplinePoint &first = points[0];
  const SplinePoint &last = points[num_points - 1];

  if (time > last.getTime())
  {
    position = last.getPoint();
    tangent = last.getTangent();
    return;
  }

  if (time < first.getTime())
  {
    position = first.getPoint();
    tangent = first.getTangent();
    return;
  }

  for (int i = 0; i < num_points - 1; i++)
  {
    if (points[i].getTime() <= time && time <= points[i + 1].getTime())
    {
      float ui =
              (time - points[i].getTime()) / (points[i+1].getTime() - points[i].getTime());
      tangent = vec3(points[i].getTangent() * (1 - ui) +
                     points[i + 1].getTangent() * ui);
      position = vec3(
              (2 * ui * ui * ui - 3 * ui * ui + 1) * points[i].getPoint() +
              (ui * ui * ui - 2 * ui * ui + ui) * points[i].getTangent()
              + (-2 * ui * ui * ui + 3 * ui * ui) * points[i + 1].getPoint() +
              (ui * ui * ui - ui * ui) * points[i + 1].getTangent());

//      position= vec3((2 *ui*ui*ui - 3f * glm::pow(ui,2) + 1) * points[i].getPoint() + (glm::pow(ui,3) - 2f * glm::pow(ui,2) + ui) * points[i].getTangent()
//                     + (-2 * glm::pow(ui,3) + 3f * glm::pow(ui,2)) * points[i + 1].getPoint() + (glm::pow(ui,3) - glm::pow(ui,2)) * points[i + 1].getTangent());
    }
  }

}