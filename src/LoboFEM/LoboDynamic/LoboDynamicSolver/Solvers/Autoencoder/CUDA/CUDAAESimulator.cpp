#include "CUDAAESimulator.h"

#include "imgui.h"
#include "LoboDynamic/LoboDynamic.h"

#include "CUDAAETimeIntegration.h"
#include "CUDAAEKineticModel.h"
#include "CUDAAEAutoDiffModel.h"

#include "LoboDynamic/LoboKeras/CUDA/CUDAKerasModel.h"

Lobo::CUDAAESimulator::CUDAAESimulator(Lobo::LoboDynamicScene *parent_scene_) : DynamicSimulator(parent_scene_)
{
    constrainmodel = NULL;
    collisionmodel = NULL;
    hyperelastic_model = NULL;
    kinetic_model = NULL;
    time_integration = NULL;
    ae_ad_model = NULL;
    keras_encoder = NULL;
}

Lobo::CUDAAESimulator::~CUDAAESimulator()
{
    delete constrainmodel;
    delete collisionmodel;
    delete hyperelastic_model;
    delete kinetic_model;
    delete time_integration;
    delete ae_ad_model;
    //delete keras_encoder;
}

void Lobo::CUDAAESimulator::drawImGui()
{
    ImGui::Text("CUDAAESimulator solver");
    DynamicSimulator::drawImGui();
}

void Lobo::CUDAAESimulator::runXMLscript(pugi::xml_node &solver_node)
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

    if (solver_node.child("AEAutoDiffModel"))
    {
        pugi::xml_node modelnode = solver_node.child("AEAutoDiffModel");
        std::string path = modelnode.attribute("path").as_string();
        path = Lobo::getPath(path.c_str());
        double min = modelnode.attribute("min").as_double();
        double scale = modelnode.attribute("scale").as_double();
        ae_ad_model = new CUDAAEAutoDiffModel(path.c_str(), min, scale);
        ae_ad_model->initModel();
    }

    if (solver_node.child("KineticModel"))
    {
        pugi::xml_node modelnode = solver_node.child("KineticModel");
        kinetic_model = new Lobo::CUDAAEKineticModel(parent_scene, bind_tetMesh, ae_ad_model, hyperelastic_model, constrainmodel, collisionmodel);
    }

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

            int full_DOFs = bind_tetMesh->getNumVertex() * 3;
            time_integration = new Lobo::CUDAAETimeIntegration(
                kinetic_model, full_DOFs, damping_ratio,
                time_step, skip_step, flags);
            time_integration->runXMLscript(modelnode);
        }
    }

    if (solver_node.attribute("precompute").as_bool())
    {
        precompute();

    }
}

void Lobo::CUDAAESimulator::precompute()
{
    DynamicSimulator::precompute();

    kinetic_model->precompute(); // will also precompute and update
                                 // tetmesh
    //hyperelastic_model->precompute();
    time_integration->precompute();

    int R = bind_tetMesh->getNumVertex() * 3;
    Eigen::VectorXd rest_shape(R);
    rest_shape.setZero();

    double mesh_min = ae_ad_model->keras_model_CUDA->getData_min();
    double mesh_scale = ae_ad_model->keras_model_CUDA->getData_scale();

    for (int i = 0; i < R; i++)
    {
        rest_shape.data()[i] = (rest_shape.data()[i] - mesh_min) / mesh_scale;
    }

    std::string encoder_path = Lobo::getPath("NN/encoder.txt");
    
    keras_encoder = new CUDAKerasModel();
    keras_encoder->loadNN(encoder_path.c_str());
    Eigen::VectorXd rest_latents(keras_encoder->getOutput());
	keras_encoder->predict(rest_shape.data(),rest_latents.data());
    time_integration->setInitLatentsq(rest_latents);
    std::cout<<"==precompute=="<<std::endl;
}

void Lobo::CUDAAESimulator::stepForward()
{
    int step = time_integration->step;
    double scale = std::sin(step * 0.1) * 0.4;

    if (step % 400 > 200)
    {
        scale = 0.0;
    }
    
    kinetic_model->external_forces = kinetic_model->gravity_force * scale;

    //kinetic_model->external_forces.setZero();
    kinetic_model->external_forces+=bind_tetMesh->tet_vertice_force*5.0;

    time_integration->stepFoward();

    bind_tetMesh->updateTetVertices(&(time_integration->q));
}

int Lobo::CUDAAESimulator::getCurStep()
{
    int tmp = 0;
    if (time_integration)
    {
        tmp = time_integration->step;
    }
    return tmp;
}