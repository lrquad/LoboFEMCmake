#include "HyperelasticModel.h"

Lobo::HyperelasticModel::HyperelasticModel(Lobo::LoboDynamicScene *scene_, Lobo::LoboTetMesh *tetmesh_):scene(scene_),tetmesh(tetmesh_)
{

}

Lobo::HyperelasticModel::~HyperelasticModel()
{
}

void Lobo::HyperelasticModel::precompute()
{
    this->tetmesh->precomputeElementData();// will check if already precomptued inside the function
    

}