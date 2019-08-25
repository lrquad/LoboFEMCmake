#pragma once
#include "LoboDynamic/LoboDynamicSolver/DynamicSolver.h"

namespace Lobo
{
    class FullspaceSolver:public DynamicSolver
    {
        public:
        FullspaceSolver(Lobo::LoboDynamicScene* parent_scene);
        ~FullspaceSolver();


        virtual void drawImGui();
        
        virtual void runXMLscript(pugi::xml_node &solver_node);


    };
}