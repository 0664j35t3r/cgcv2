#ifndef Model_H_INCLUDED
#define Model_H_INCLUDED

#include <vector>
#include "Mesh.h"
#include "Material.h"
#include "Joint.h"
#include "Animation.h"

class Model
{
public:
  Model();
  virtual ~Model();

  void addMesh(const Mesh& mesh);
  void addMeshes(const std::vector<Mesh>& meshes);
  const Mesh& getMesh(size_t index) const;
  const std::vector<Mesh>& getMeshes() const;
  size_t getMeshCount() const;

  void addMaterial(const Material& material);
  void addMaterials(const std::vector<Material>& materials);
  const Material& getMaterial(size_t index) const;
  const std::vector<Material>& getMaterials() const;
  size_t getMaterialCount() const;

  void addJoint(const Joint& joint);
  void addJoints(const std::vector<Joint>& joints);
  const Joint& getJoint(size_t index) const;
  const std::vector<Joint>& getJoints() const;
  size_t getJointCount() const;

  void addAnimation(const Animation& action);
  const Animation& getAnimation(size_t index) const;
  size_t getActionCount() const;

protected:
  std::vector<Mesh> meshes_;
  std::vector<Material> materials_;
  std::vector<Joint> joints_;
  std::vector<Animation> actions_;

private:
  Model(const Model* model);
};


#endif  // Model_H_INCLUDED


