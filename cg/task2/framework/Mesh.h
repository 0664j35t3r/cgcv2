#ifndef Mesh_H_INCLUDED
#define Mesh_H_INCLUDED

#include <vector>
#include <map>
#include <glm/glm.hpp>

class Joint;

class Mesh
{
public:
  Mesh();
  Mesh(const Mesh& mesh);
  virtual ~Mesh();

  void addVertex(const glm::vec3& vertex);
  void addVertices(const std::vector<glm::vec3>& vertices);
  void setVertex(size_t index, const glm::vec3& vertex);
  const glm::vec3& getVertex(size_t index) const;
  const std::vector<glm::vec3>& getVertices() const;
  size_t getVertexCount() const;

  void addNormal(const glm::vec3& normal);
  void addNormals(const std::vector<glm::vec3>& normals);
  void setNormal(size_t index, const glm::vec3& normal);
  const glm::vec3& getNormal(size_t index) const;
  const std::vector<glm::vec3>& getNormals() const;
  size_t getNormalCount() const;

  void addTriangle(const glm::ivec3& triangle);
  void addTriangles(const std::vector<glm::ivec3>& triangles);
  const glm::ivec3& getTriangle(size_t index) const;
  const std::vector<glm::ivec3>& getTriangles() const;
  size_t getTriangleCount() const;

  void setMaterial(size_t material_index);
  size_t getMaterial() const;

  void addWeight(size_t vertex_index, float weight);
  void addWeights(size_t vertex_index, const std::vector<float>& weights);
  float getWeight(size_t vertex_index, size_t index) const;
  const std::vector<float>& getWeights(size_t vertex_index) const;
  const std::vector<std::vector<float> >& getWeights() const;
  size_t getWeightCount(size_t vertex_index) const;

  void addJoint(size_t vertex_index, size_t joint);
  void addJoints(size_t vertex_index, const std::vector<size_t>& joints);
  size_t getJoint(size_t vertex_index, size_t index) const;
  const std::vector<size_t>& getJoints(size_t vertex_index) const;
  const std::vector<std::vector<size_t> >& getJoints() const;
  size_t getJointCount(size_t vertex_index) const;

  void allocateSpace(size_t vertex_count);

protected:
  std::vector<glm::vec3> vertices_;
  std::vector<glm::vec3> normals_;
  std::vector<glm::ivec3> triangles_;
  size_t material_;
  std::vector<std::vector<float> > weights_;
  std::vector<std::vector<size_t> > joints_;
};


#endif  // Mesh_H_INCLUDED

