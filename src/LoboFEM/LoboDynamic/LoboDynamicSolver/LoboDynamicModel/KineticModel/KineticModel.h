#pragma once
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "LoboDynamic/LoboDynamicScene.h"
#include "LoboDynamic/LoboDynamicSolver/LoboDynamicModel/DynamicModel.h"

namespace Lobo
{

class HyperelasticModel;
class ConstrainModel;

class KineticModel : public DynamicModel
{
private:
    /* data */
public:
    KineticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_,HyperelasticModel* elastic_model_,ConstrainModel* constrain_model_);
    ~KineticModel();

    //precomptue final sparse matrix topology
    virtual void precompute();

    virtual void runXMLscript(pugi::xml_node &xml_node);

    virtual void computeEnergySparse(double *free_variables, double *energy, double *jacobi, Eigen::SparseMatrix<double> *hessian, int computationflags){};
    virtual void computeEnergyDense(double *free_variables, double *energy, double *jacobi, double *hessian, int computationflags){};

protected:


    LoboDynamicScene *scene;
    LoboTetMesh *tetmesh;

    HyperelasticModel* hyperelasticmodel;
    ConstrainModel* constrainmodel;

};
} // namespace Lobo