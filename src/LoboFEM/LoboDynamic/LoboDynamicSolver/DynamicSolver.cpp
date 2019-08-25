#include "DynamicSolver.h"
#include "LoboDynamic/LoboDynamicScene.h"

Lobo::DynamicSolver::DynamicSolver(LoboDynamicScene* parent_scene_)
    : parent_scene(parent_scene_) {}

Lobo::DynamicSolver::~DynamicSolver() {}


void Lobo::DynamicSolver::drawImGui()
{

}

void Lobo::DynamicSolver::runXMLscript(pugi::xml_node& solver_node) {



}