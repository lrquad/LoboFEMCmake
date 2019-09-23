#pragma once
#include "LoboDynamic/LoboDynamic.h"

namespace Lobo
{
    
class CUDAAEAutoDiffModel;

class CUDAAEKineticModel : public KineticModel
{
public:
    CUDAAEKineticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_,CUDAAEAutoDiffModel* ae_ad_model_, HyperelasticModel *elastic_model_, ConstrainModel *constrain_model_, CollisionModel *collisionmodel_);
    ~CUDAAEKineticModel();

    virtual void precompute();
    virtual void computeEnergyDense(Eigen::VectorXd *free_variables, double *energy, Eigen::VectorXd *jacobi, Eigen::MatrixXd *hessian, int computationflags);

    int getNum_latents();
    void AE_value(double* latents,double*output);

protected:

    CUDAAEAutoDiffModel* ae_ad_model;
    Eigen::VectorXd q;
    Eigen::VectorXd internalforce;
    int full_DoFs;
};
} // namespace Lobo