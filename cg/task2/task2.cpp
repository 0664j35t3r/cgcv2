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


float global_time = 0;


void printMatrix()
{

}


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

  //  std::cout << joints.at(5).getBasePoseMatrix()<< std::endl;

  //  Keyframe::getTranslation(joint)
  //  Keyframe::getRotation(joint)
  //  glm::slerp
  //  glm::translate
  //  glm::mat4_cast

  //  std::cout << "joints " << joints.size() << std::endl;
  //  std::cout << "keyfra " << keyframes.size() << std::endl;
  //  std::cout << "keyfra " << keyframes[3].getTime() << std::endl;

  //  if(global_time < time)
  //    global_time = time;

  //  std::cout << global_time << std::endl;

  glm::mat4 relative_trans;
  glm::mat4 relative_rot;

  for (int i = 0; i < keyframes.size() - 1; i++)
  {
    if(i <  keyframes.size() - 2)
      if(keyframes[i].getTime() >= time && keyframes[i+1].getTime() < time)
      {
        glm::translate(relative_trans, keyframes[i].relativeTo(keyframes[i+1]).getTranslation(joints[i].getID()));
        relative_rot = glm::mat4_cast(glm::slerp(keyframes[i].getRotation(joints[i].getID()), keyframes[i+1].getRotation(joints[i].getID()), (time - keyframes[i].getTime()) / (keyframes[i+1].getTime() - keyframes[i].getTime())));
        if (joints[i].getParent() < 0) // root
          joint_transformations[i] = relative_rot * relative_trans;
        else // childs
          joint_transformations[i] = joint_transformations[joints[i].getParent()] * relative_rot * relative_trans;

        for (auto joint : joints)
        {
          cout << "joints " << joint.getParent() << endl;
        }
      }
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

  std::fill(normals.begin(), normals.begin() + vertices.size(),
      glm::vec3(0.0f));
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

  const SplinePoint& first = points[0];
  const SplinePoint& last = points[num_points - 1];
  
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

  position = glm::vec3(0);
  tangent = glm::vec3(0);
}
