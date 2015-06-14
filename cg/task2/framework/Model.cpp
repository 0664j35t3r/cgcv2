#include "Model.h"

Model::Model()
{
}

Model::~Model()
{
}

void Model::addMesh(const Mesh& mesh)
{
  meshes_.push_back(mesh);
}

void Model::addMeshes(const std::vector<Mesh>& meshes)
{
  meshes_.insert(meshes_.end(), meshes.begin(), meshes.end());
}

const Mesh& Model::getMesh(size_t index) const
{
  return meshes_[index];
}

const std::vector<Mesh>& Model::getMeshes() const
{
  return meshes_;
}

size_t Model::getMeshCount() const
{
  return meshes_.size();
}

void Model::addMaterial(const Material& material)
{
  materials_.push_back(material);
}

void Model::addMaterials(const std::vector<Material>& materials)
{
  materials_.insert(materials_.end(), materials.begin(), materials.end());
}

const Material& Model::getMaterial(size_t index) const
{
  return materials_[index];
}

const std::vector<Material>& Model::getMaterials() const
{
  return materials_;
}

size_t Model::getMaterialCount() const
{
  return materials_.size();
}

void Model::addJoint(const Joint& joint)
{
  joints_.push_back(joint);
}

void Model::addJoints(const std::vector<Joint>& joints)
{
  joints_.insert(joints_.end(), joints.begin(), joints.end());
}

const Joint& Model::getJoint(size_t index) const
{
  return joints_[index];
}

const std::vector<Joint>& Model::getJoints() const
{
  return joints_;
}

size_t Model::getJointCount() const
{
  return joints_.size();
}

void Model::addAnimation(const Animation& action)
{
  actions_.push_back(action);
}

const Animation& Model::getAnimation(size_t index) const
{
  return actions_[index];
}

size_t Model::getActionCount() const
{
  return actions_.size();
}
