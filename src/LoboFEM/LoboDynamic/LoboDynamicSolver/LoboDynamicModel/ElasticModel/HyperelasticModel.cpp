#include "HyperelasticModel.h"
#include "LoboDynamic/LoboDynamic.h"

Lobo::HyperelasticModel::HyperelasticModel(Lobo::LoboDynamicScene *scene_, Lobo::LoboTetMesh *tetmesh_):scene(scene_),tetmesh(tetmesh_)
{
    elastic_material = NULL;
    is_sparse_sovler = true;
    num_DOFs= tetmesh_->getNumVertex()*3;
}

Lobo::HyperelasticModel::~HyperelasticModel()
{
    delete elastic_material;
}

void Lobo::HyperelasticModel::precompute()
{
    this->tetmesh->precomputeElementData();// will check if already precomptued inside the function
    
    // Dm Dm_inverse per element
    // volume per element
    
    //precompute sparse matrix topology

}

void Lobo::HyperelasticModel::runXMLscript(pugi::xml_node &xml_node)
{
    this->tetmesh->precomputeElementData();

    if(xml_node.child("Material"))
    {
        if(strcmp("StVK",xml_node.child("Material").attribute("type").as_string())==0)
        {
            this->elastic_material = new TypeStVKMaterial<double>(tetmesh, 1, 500.0);
            materialtype = "StVK";
        }
    }
}