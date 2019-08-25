#include "FullspaceSolver.h"
#include "LoboDynamic/LoboDynamic.h"
#include "imgui.h"

Lobo::FullspaceSolver::FullspaceSolver(Lobo::LoboDynamicScene *parent_scene_) : DynamicSolver(parent_scene_)
{
    hyperelastic_model = NULL;
}

Lobo::FullspaceSolver::~FullspaceSolver()
{
    delete hyperelastic_model;
}

void Lobo::FullspaceSolver::drawImGui()
{
    ImGui::Text("Fullspace solver");
    DynamicSolver::drawImGui();
    ImGui::Text("Material %s",hyperelastic_model->materialtype.c_str());
}

void Lobo::FullspaceSolver::runXMLscript(pugi::xml_node &solver_node)
{
    DynamicSolver::runXMLscript(solver_node);

    if(solver_node.child("HyperelasticModel"))
    {
        pugi::xml_node modelnode = solver_node.child("HyperelasticModel");
        hyperelastic_model = new Lobo::HyperelasticModel(parent_scene,bind_tetMesh);
        hyperelastic_model->runXMLscript(modelnode);
        models.push_back(hyperelastic_model);
    }

    if(solver_node.child("KineticModel"))
    {
        pugi::xml_node modelnode = solver_node.child("KineticModel");
        kinetic_model = new Lobo::KineticModel(parent_scene,bind_tetMesh,hyperelastic_model,constrainmodel);
        kinetic_model->runXMLscript(modelnode);
        models.push_back(kinetic_model);
    }

}

void Lobo::FullspaceSolver::precompute()
{
    DynamicSolver::precompute();
    

    hyperelastic_model->precompute(); //will also precompute and update tetmesh

}
