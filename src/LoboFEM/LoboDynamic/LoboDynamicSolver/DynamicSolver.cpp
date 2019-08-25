#include "DynamicSolver.h"
#include "LoboDynamic/LoboDynamicScene.h"
#include "imgui.h"
#include "LoboVolumtricMesh/LoboTetMesh.h"
#include "LoboDynamic/LoboDynamic.h"

Lobo::DynamicSolver::DynamicSolver(LoboDynamicScene *parent_scene_)
    : parent_scene(parent_scene_)
{
    bind_tetMesh = NULL;
    target_tetmesh_id = -1;
    constrainmodel = NULL;
}

Lobo::DynamicSolver::~DynamicSolver() {
    delete constrainmodel;
}

void Lobo::DynamicSolver::drawImGui()
{
    ImGui::Text("num dynamic models: %d", models.size());

    if (bind_tetMesh != NULL)
    {
        ImGui::Text("Target Tet mesh id: %d", target_tetmesh_id);
        ImGui::Text("Tet mesh name %s", bind_tetMesh->filebase.c_str());
    }
    else
    {
        ImGui::Text("Target Tet mesh id: %d", target_tetmesh_id);
    }
}

void Lobo::DynamicSolver::runXMLscript(pugi::xml_node &solver_node)
{

    if (solver_node.child("TargetTetMesh"))
    {
        target_tetmesh_id = solver_node.child("TargetTetMesh").attribute("id").as_int();
        bind_tetMesh = this->parent_scene->tetmesh_in_scene[target_tetmesh_id];
    }

    if(solver_node.child("ConstraintModel"))
    {
        double weight = solver_node.child("ConstraintModel").attribute("weight").as_double();
        std::string filepath = solver_node.child("ConstraintModel").child("ConstraintsFile").text().as_string();
        constrainmodel = new Lobo::ConstrainModel();
        models.push_back(constrainmodel);
    }

    if(solver_node.child("HyperelasticModel"))
    {

    }
}