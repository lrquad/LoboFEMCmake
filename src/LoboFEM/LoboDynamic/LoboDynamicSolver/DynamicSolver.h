#pragma once

#include <vector>
#include <cstring>
#include "Utils/pugixml/pugixml.hpp"

namespace Lobo
{
    class DynamicModel;
    class LoboDynamicScene;
    class LoboTetMesh;


    class ConstrainModel;
    class CollisionModel;

    class DynamicSolver
    {
        public:
        DynamicSolver(LoboDynamicScene* parent_scene);
        ~DynamicSolver();

        virtual void drawImGui();

        virtual void stepForward() = 0;
        virtual int getCurStep() = 0;

        virtual void runXMLscript(pugi::xml_node &solver_node);

        virtual void precompute();

        virtual void setBindTetmeshFromScene(int tetmesh_id);

        std::vector<DynamicModel*> models;
        
        //general models
        ConstrainModel* constrainmodel;
        CollisionModel* collisionmodel;
        LoboDynamicScene* parent_scene;
        int target_tetmesh_id;
        LoboTetMesh* bind_tetMesh;
    };
}
