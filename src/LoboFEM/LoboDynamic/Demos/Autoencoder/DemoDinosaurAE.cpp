#include "DemoDinosaurAE.h"
#include "imgui.h"
#include "LoboDynamic/LoboDynamic.h"

Lobo::DemoDinosaurAE::DemoDinosaurAE(Lobo::LoboDynamicScene* parent_scene_)
    : DynamicSolver(parent_scene_) {
    constrainmodel = NULL;
    collisionmodel = NULL;
    hyperelastic_model = NULL;
    kinetic_model = NULL;
    time_integraion = NULL;
}

Lobo::DemoDinosaurAE::~DemoDinosaurAE() {
    delete constrainmodel;
    delete collisionmodel;
    delete hyperelastic_model;
    delete kinetic_model;
    delete time_integraion;
}

void Lobo::DemoDinosaurAE::drawImGui() {
    ImGui::Text("DemoDinosaurAE solver");
    DynamicSolver::drawImGui();
}

void Lobo::DemoDinosaurAE::runXMLscript(pugi::xml_node& solver_node) {
    DynamicSolver::runXMLscript(solver_node);

    if (solver_node.child("ConstraintModel")) {
        pugi::xml_node model_node = solver_node.child("ConstraintModel");

        double weight = model_node.attribute("weight").as_double();
        constrainmodel = new Lobo::ConstrainModel(bind_tetMesh);
        constrainmodel->runXMLscript(model_node);
        models.push_back(constrainmodel);
    }

    if (solver_node.child("CollisionModel")) {
        pugi::xml_node model_node = solver_node.child("CollisionModel");
        collisionmodel = new Lobo::CollisionModel(bind_tetMesh);
        collisionmodel->runXMLscript(model_node);
        models.push_back(collisionmodel);
    }

    if (solver_node.child("HyperelasticModel")) {
        pugi::xml_node modelnode = solver_node.child("HyperelasticModel");
        hyperelastic_model =
            new Lobo::HyperelasticModel(parent_scene, bind_tetMesh);
        hyperelastic_model->runXMLscript(modelnode);
        models.push_back(hyperelastic_model);
    }

    if (solver_node.child("KineticModel")) {
        pugi::xml_node modelnode = solver_node.child("KineticModel");
    }
}

 void Lobo::DemoDinosaurAE::precompute()
 {

 }
 void Lobo::DemoDinosaurAE::stepForward()
 {

 }
 int Lobo::DemoDinosaurAE::getCurStep()
 {
     
 }