#include "GraspFingerSimulator.h"
#include "LoboDynamic/LoboDynamic.h"
#include "OpenGLutils/LoboScene.h"

Lobo::GraspFingerSimulator::GraspFingerSimulator(
    Lobo::LoboDynamicScene *parent_scene_)
    : DynamicSimulator(parent_scene_) {
    contact_index = NULL;
    trimesh = NULL;
    simulation_step = 0;
}

Lobo::GraspFingerSimulator::~GraspFingerSimulator() {}

void Lobo::GraspFingerSimulator::drawImGui() {
    ImGui::Text("Finger mover");
    DynamicSimulator::drawImGui();
}

void Lobo::GraspFingerSimulator::runXMLscript(pugi::xml_node &solver_node) {
    DynamicSimulator::runXMLscript(solver_node);
    if (solver_node.child("TrimeshId")) {
        int triid = solver_node.child("TrimeshId").text().as_int();
        trimesh = this->parent_scene->scene->getMesh(triid);
    }
    if (solver_node.child("ContactPoints")) {
        contact_index = solver_node.child("ContactPoints").text().as_int();
        std::cout << "contact index" << contact_index << std::endl;
    }

    if (solver_node.attribute("precompute").as_bool())
    {
        precompute();
    }
}

void Lobo::GraspFingerSimulator::precompute() {
    DynamicSimulator::precompute();
    std::cout<<"Grasp simulator precomptued" << std::endl;

    int numtrinode = trimesh->attrib.vertices.size() / 3;
    Eigen::VectorXd buffer(numtrinode * 3);
    trimesh->getCurVertices(buffer.data());
    
    Eigen::Vector3d tri_mesh_center;
    tri_mesh_center.setZero();
    for(int i=0;i<numtrinode;i++)
    {
        tri_mesh_center.data()[0]+=buffer.data()[i*3+0];
        tri_mesh_center.data()[1]+=buffer.data()[i*3+1];
        tri_mesh_center.data()[2]+=buffer.data()[i*3+2];
    }
    tri_mesh_center/=numtrinode;

    //move to index 
    Eigen::Vector3d target_position = this->bind_tetMesh->getNodeRestPosition(contact_index);

    Eigen::Vector3d translation_ = target_position - tri_mesh_center;
    //compute node normal direction
    


    for(int i=0;i<numtrinode;i++)
    {
        buffer.data()[i*3+0]+=translation_.data()[0];
        buffer.data()[i*3+1]+=translation_.data()[1];
        buffer.data()[i*3+2]+=translation_.data()[2];
    }

    trimesh->updateVertices(buffer.data());
    

    
}

void Lobo::GraspFingerSimulator::stepForward() {

    int numtrinode = trimesh->attrib.vertices.size() / 3;
    Eigen::VectorXd buffer(numtrinode * 3);
    buffer.setZero();
    trimesh->getCurVertices(buffer.data());
    for (int i = 0; i < numtrinode; i++) {
        buffer.data()[i*3+1] += -0.01;
    }
    trimesh->updateVertices(buffer.data());


    simulation_step++;
}

int Lobo::GraspFingerSimulator::getCurStep() {
    
    return simulation_step;
}