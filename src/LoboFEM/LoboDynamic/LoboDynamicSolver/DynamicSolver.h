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

    class DynamicSolver
    {
        public:
        DynamicSolver(LoboDynamicScene* parent_scene);
        ~DynamicSolver();

        virtual void drawImGui();

        virtual void runXMLscript(pugi::xml_node &solver_node);

        virtual void precompute();

        std::vector<DynamicModel*> models;
        
        //general models
        ConstrainModel* constrainmodel;


        LoboDynamicScene* parent_scene;
        int target_tetmesh_id;
        LoboTetMesh* bind_tetMesh;


    };
}
