#include "AEKineticModel.h"

Lobo::AEKineticModel::AEKineticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_, AEElasticModel *ae_elastic_model_,HyperelasticModel* elastic_model_, ConstrainModel *constrain_model_, CollisionModel *collisionmodel_):KineticModel(scene_,tetmesh_,elastic_model_,constrain_model_,collisionmodel_)
{
    this->ae_elastic_model = ae_elastic_model_;
}

Lobo::AEKineticModel::~AEKineticModel()
{

}

void Lobo::AEKineticModel::precompute()
{

}