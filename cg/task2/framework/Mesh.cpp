#include "Mesh.h"
#include "Joint.h"

Mesh::Mesh() : material_(0)
{
}

Mesh::Mesh(const Mesh& mesh)
    : vertices_(mesh.vertices_), normals_(mesh.normals_), triangles_(mesh.triangles_), material_(mesh.material_), weights_(mesh.weights_), joints_(mesh.joints_)
{
}

Mesh::~Mesh()
{
}

void Mesh::allocateSpace(size_t vertex_count)
{
  weights_.resize(vertex_count);
  joints_.resize(vertex_count);
}

void Mesh::addVertex(const glm::vec3& vertex)
{
  vertices_.push_back(vertex);
}

void Mesh::addVertices(const std::vector<glm::vec3>& vertices)
{
  vertices_.insert(vertices_.end(), vertices.begin(), vertices.end());
}

void Mesh::setVertex(size_t index, const glm::vec3& vertex)
{
  vertices_[index] = vertex;
}

const glm::vec3& Mesh::getVertex(size_t index) const
{
  return vertices_[index];
}

const std::vector<glm::vec3>& Mesh::getVertices() const
{
  return vertices_;
}

size_t Mesh::getVertexCount() const
{
  return vertices_.size();
}

void Mesh::addNormal(const glm::vec3& normal)
{
  normals_.push_back(normal);
}

void Mesh::addNormals(const std::vector<glm::vec3>& normals)
{
  normals_.insert(normals_.end(), normals.begin(), normals.end());
}

void Mesh::setNormal(size_t index, const glm::vec3& normal)
{
  normals_[index] = normal;
}

const glm::vec3& Mesh::getNormal(size_t index) const
{
  return normals_[index];
}

const std::vector<glm::vec3>& Mesh::getNormals() const
{
  return normals_;
}

size_t Mesh::getNormalCount() const
{
  return normals_.size();
}

void Mesh::addTriangle(const glm::ivec3& triangle)
{
  triangles_.push_back(triangle);
}

void Mesh::addTriangles(const std::vector<glm::ivec3>& triangles)
{
  triangles_.insert(triangles_.end(), triangles.begin(), triangles.end());
}

const glm::ivec3& Mesh::getTriangle(size_t index) const
{
  return triangles_[index];
}

const std::vector<glm::ivec3>& Mesh::getTriangles() const
{
  return triangles_;
}

size_t Mesh::getTriangleCount() const
{
  return triangles_.size();
}

void Mesh::setMaterial(size_t material_index)
{
  material_ = material_index;
}

size_t Mesh::getMaterial() const
{
  return material_;
}

void Mesh::addWeight(size_t vertex_index, float weight)
{
  weights_[vertex_index].push_back(weight);
}

void Mesh::addWeights(size_t vertex_index,
                      const std::vector<float>& weights)
{
  weights_[vertex_index].insert(weights_[vertex_index].end(),
                                weights.begin(), weights.end());
}

float Mesh::getWeight(size_t vertex_index, size_t index) const
{
  return weights_[vertex_index][index];
}

const std::vector<float>& Mesh::getWeights(size_t vertex_index) const
{
  return weights_[vertex_index];
}

const std::vector<std::vector<float>>& Mesh::getWeights() const
{
  return weights_;
}

void Mesh::addJoint(size_t vertex_index, size_t joint)
{
  joints_[vertex_index].push_back(joint);
}

void Mesh::addJoints(size_t vertex_index, const std::vector<size_t>& joints)
{
  joints_[vertex_index].insert(joints_[vertex_index].end(), joints.begin(), joints.end());
}

size_t Mesh::getJoint(size_t vertex_index, size_t index) const
{
  return joints_[vertex_index][index];
}

size_t Mesh::getWeightCount(size_t vertex_index) const
{
  return weights_[vertex_index].size();
}

const std::vector<size_t>& Mesh::getJoints(size_t vertex_index) const
{
  return joints_[vertex_index];
}

const std::vector<std::vector<size_t>>& Mesh::getJoints() const
{
  return joints_;
}

size_t Mesh::getJointCount(size_t vertex_index) const
{
  return joints_[vertex_index].size();
}
