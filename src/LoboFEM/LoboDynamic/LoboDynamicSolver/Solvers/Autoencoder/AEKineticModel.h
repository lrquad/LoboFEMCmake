#pragma once
#include "LoboDynamic/LoboDynamic.h"
template<class TYPE> class AEAutoDiffModel;
namespace Lobo
{
class AEKineticModel : public KineticModel
{
public:

    AEKineticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_,AEAutoDiffModel<double>* ae_ad_model_, HyperelasticModel *elastic_model_, ConstrainModel *constrain_model_, CollisionModel *collisionmodel_);
    ~AEKineticModel();

    virtual void precompute();

    virtual void computeEnergyDense(Eigen::VectorXd*free_variables, double *energy, Eigen::VectorXd*jacobi, Eigen::MatrixXd*hessian, int computationflags);

    AEAutoDiffModel<double>* ae_ad_model;

protected:

    Eigen::VectorXd q;
    Eigen::VectorXd internalforce;
    int full_DoFs;
};
} // namespace Lobo
