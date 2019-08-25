#pragma once
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "LoboDynamic/LoboDynamicScene.h"
#include <Eigen/Dense>

namespace Lobo
{




class HyperelasticModel
{
private:
    /* data */
public:
    HyperelasticModel(LoboDynamicScene *scene_, LoboTetMesh *tetmesh_);
    ~HyperelasticModel();

    virtual void precompute();

protected:
    
    LoboDynamicScene* scene;
    LoboTetMesh* tetmesh;

    
   

};
} // namespace Lobo