#pragma once
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "LoboDynamic/LoboDynamicScene.h"
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/DynamicModel.h"

#include <Eigen/Dense>

template <class TYPE>
class TypeIsotropicMaterial;

namespace Lobo
{

class HyperelasticModel : public DynamicModel
{
private:
    /* data */
public:
    HyperelasticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_);
    ~HyperelasticModel();

    virtual void precompute();

    virtual void runXMLscript(pugi::xml_node &xml_node);

    virtual void computeEnergySparse(double *free_variables, double *energy, double *jacobi, Eigen::SparseMatrix<double> *hessian, int computationflags){};
    virtual void computeEnergyDense(double *free_variables, double *energy, double *jacobi, double *hessian, int computationflags){};

    std::string materialtype;

protected:

    LoboDynamicScene *scene;
    LoboTetMesh *tetmesh;
    TypeIsotropicMaterial<double> *elastic_material;

};
} // namespace Lobo