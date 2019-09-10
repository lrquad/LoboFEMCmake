#include "FullspaceSolver.h"
#include "LoboDynamic/LoboDynamic.h"
#include "imgui.h"

Lobo::FullspaceSolver::FullspaceSolver(Lobo::LoboDynamicScene *parent_scene_)
    : DynamicSolver(parent_scene_)
{
    hyperelastic_model = NULL;
    time_integraion = NULL;
    constrainmodel = NULL;
    collisionmodel = NULL;
}

Lobo::FullspaceSolver::~FullspaceSolver()
{
    delete hyperelastic_model;
    delete time_integraion;
    delete constrainmodel;
    delete collisionmodel;
}

void Lobo::FullspaceSolver::drawImGui()
{
    ImGui::Text("Fullspace solver");
    DynamicSolver::drawImGui();

    

    if (hyperelastic_model)
    {
        ImGui::Text("Material %s", hyperelastic_model->materialtype.c_str());
        ImGui::Text("isinvertible %s", hyperelastic_model->isinvertible ? "true" : "false");
        ImGui::Text("useMCSFD %s", hyperelastic_model->useMCSFD ? "true" : "false");
        ImGui::Text("Youngsmodulues %.4f", bind_tetMesh->getElementMaterial(0)->getE());
    }
    ImGui::Separator();
    if (time_integraion)
    {
        ImGui::Text("Timestep %.4f", time_integraion->timestep);
        ImGui::Text("Damping_ratio %.4f", time_integraion->damping_ratio);
        ImGui::Text("Step %d", time_integraion->step);
        ImGui::Text("Skip steps %d", time_integraion->skip_steps);
    }
}

void Lobo::FullspaceSolver::runXMLscript(pugi::xml_node &solver_node)
{
    DynamicSolver::runXMLscript(solver_node);

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

    if (solver_node.child("KineticModel"))
    {
        pugi::xml_node modelnode = solver_node.child("KineticModel");
        
        if(!modelnode.attribute("method"))
        {
            kinetic_model = new Lobo::KineticModel(
            parent_scene, bind_tetMesh, hyperelastic_model, constrainmodel, collisionmodel);
        }

        if (strcmp(modelnode.attribute("method").as_string(),
                   "KineticModel") == 0)
        {
            kinetic_model = new Lobo::KineticModel(
                parent_scene, bind_tetMesh, hyperelastic_model, constrainmodel, collisionmodel);
        }

        if (strcmp(modelnode.attribute("method").as_string(),
                   "LinearKineticModel") == 0)
        {
            kinetic_model = new Lobo::LinearKineticModel(
                parent_scene, bind_tetMesh, hyperelastic_model, constrainmodel, collisionmodel);
        }

        kinetic_model->runXMLscript(modelnode);
        models.push_back(kinetic_model);
    }

    //create time integration

    if (solver_node.child("TimeIntegration"))
    {
        pugi::xml_node modelnode = solver_node.child("TimeIntegration");

        if (strcmp(modelnode.attribute("method").as_string(),
                   "ImplicitSparse") == 0)
        {
            double damping_ratio = 0.99;
            double time_step = 0.01;
            int skip_step = 1;
            int flags = 0;

            if (modelnode.attribute("damping"))
                damping_ratio = modelnode.attribute("damping").as_double();

            if (modelnode.attribute("timestep"))
                time_step = modelnode.attribute("timestep").as_double();

            if (modelnode.attribute("skipsteps"))
                skip_step = modelnode.attribute("skipsteps").as_int();

            if (modelnode.attribute("recordq"))
                if (modelnode.attribute("recordq").as_bool())
                {
                    flags |= IntegratorFlags_recordq;
                }

            time_integraion = new Lobo::ImplicitSparseIntegration(
                kinetic_model, kinetic_model->num_DOFs, damping_ratio,
                time_step, skip_step, flags);
            time_integraion->runXMLscript(modelnode);
        }
    }

    if (solver_node.attribute("precompute").as_bool())
    {
        precompute();
    }
}

void Lobo::FullspaceSolver::precompute()
{
    DynamicSolver::precompute();

    kinetic_model->precompute(); // will also precompute and update
                                 // tetmesh

    //hyperelastic_model->precompute();
    time_integraion->precompute();
}

void Lobo::FullspaceSolver::stepForward()
{

    time_integraion->stepFoward();

    bind_tetMesh->updateTetVertices(&(time_integraion->q));

}

int Lobo::FullspaceSolver::getCurStep()
{
    int tmp = 0;
    if (time_integraion)
    {
        tmp = time_integraion->step;
    }
    return tmp;
}