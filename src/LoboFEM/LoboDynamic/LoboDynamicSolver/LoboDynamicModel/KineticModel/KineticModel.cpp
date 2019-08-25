#include "KineticModel.h"

Lobo::KineticModel::KineticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_, HyperelasticModel *elastic_model_, ConstrainModel *constrain_model_) : scene(scene_), tetmesh(tetmesh_), hyperelasticmodel(elastic_model_), constrainmodel(constrain_model_)
{
}

Lobo::KineticModel::~KineticModel()
{
}

//precomptue final sparse matrix topology
void Lobo::KineticModel::precompute()
{

}

void Lobo::KineticModel::runXMLscript(pugi::xml_node &xml_node)
{
    
}