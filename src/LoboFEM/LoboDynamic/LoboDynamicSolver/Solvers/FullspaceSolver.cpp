#include "FullspaceSolver.h"
#include "LoboDynamic/LoboDynamic.h"
#include "imgui.h"

Lobo::FullspaceSolver::FullspaceSolver(Lobo::LoboDynamicScene *parent_scene_) : DynamicSolver(parent_scene_)
{
}

Lobo::FullspaceSolver::~FullspaceSolver()
{
}

void Lobo::FullspaceSolver::drawImGui()
{
    ImGui::Text("Fullspace solver");
    DynamicSolver::drawImGui();


}

void Lobo::FullspaceSolver::runXMLscript(pugi::xml_node &solver_node)
{
    DynamicSolver::runXMLscript(solver_node);

    

}
