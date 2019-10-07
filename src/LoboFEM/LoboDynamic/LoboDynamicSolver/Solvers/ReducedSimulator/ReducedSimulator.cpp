#include "ReducedSimulator.h"
#include "imgui.h"
#include "LoboDynamic/LoboDynamic.h"

Lobo::ReducedSimulator::ReducedSimulator(Lobo::LoboDynamicScene *parent_scene_)
    : DynamicSimulator(parent_scene_)
{
      constrainmodel = NULL;
    collisionmodel = NULL;
    hyperelastic_model = NULL;
}


Lobo::ReducedSimulator::~ReducedSimulator()
{
    delete constrainmodel;
    delete collisionmodel;
    delete hyperelastic_model;
}

void Lobo::ReducedSimulator::drawImGui()
{
    ImGui::Text("ReducedSimulator solver");
    DynamicSimulator::drawImGui();
}

void Lobo::ReducedSimulator::runXMLscript(pugi::xml_node &solver_node)
{
    DynamicSimulator::runXMLscript(solver_node);

    if (solver_node.child("ConstraintModel"))
    {
        pugi::xml_node model_node = solver_node.child("ConstraintModel");

        double weight = model_node.attribute("weight").as_double();
        constrainmodel = new Lobo::ConstrainModel(bind_tetMesh);
        constrainmodel->runXMLscript(model_node);
        models.push_back(constrainmodel);
    }

    if (solver_node.child("CollisionModel"))
    {
        pugi::xml_node model_node = solver_node.child("CollisionModel");
        collisionmodel = new Lobo::CollisionModel(bind_tetMesh);
        collisionmodel->runXMLscript(model_node);
        models.push_back(collisionmodel);
    }

    if (solver_node.child("HyperelasticModel"))
    {
        pugi::xml_node modelnode = solver_node.child("HyperelasticModel");
        hyperelastic_model =
            new Lobo::HyperelasticModel(parent_scene, bind_tetMesh);
        hyperelastic_model->runXMLscript(modelnode);
        models.push_back(hyperelastic_model);
    }
}

void Lobo::ReducedSimulator::precompute()
{
    DynamicSimulator::precompute();

}

void Lobo::ReducedSimulator::stepForward()
{

}

int Lobo::ReducedSimulator::getCurStep()
{
    int tmp = 0;
    
    return tmp;
}