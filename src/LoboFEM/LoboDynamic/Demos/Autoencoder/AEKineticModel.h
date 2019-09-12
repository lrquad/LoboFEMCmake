#pragma once
#include "LoboDynamic/LoboDynamic.h"

namespace Lobo
{
class AEElasticModel;

class AEKineticModel : public KineticModel
{
private:
    /* data */
public:

    AEKineticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_, AEElasticModel *ae_elastic_model_,HyperelasticModel* elastic_model_, ConstrainModel *constrain_model_, CollisionModel *collisionmodel_);
    ~AEKineticModel();

    virtual void precompute();

protected:

    AEElasticModel *ae_elastic_model;
};

} // namespace Lobo
