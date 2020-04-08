#include "GraspFingerSimulator.h"
#include "LoboDynamic/LoboDynamic.h"
#include "OpenGLutils/LoboScene.h"

Lobo::GraspFingerSimulator::GraspFingerSimulator(
    Lobo::LoboDynamicScene *parent_scene_)
    : DynamicSimulator(parent_scene_) {
    contact_index = NULL;
    trimesh = NULL;
}

Lobo::GraspFingerSimulator::~GraspFingerSimulator() {}

void Lobo::GraspFingerSimulator::drawImGui() {
    ImGui::Text("Finger mover");
    DynamicSimulator::drawImGui();
}

void Lobo::GraspFingerSimulator::runXMLscript(pugi::xml_node &solver_node) {
    // DynamicSimulator::runXMLscript(solver_node);
    if (solver_node.child("TrimeshId")) {
        int triid = solver_node.child("TrimeshId").text().as_int();
        trimesh = this->parent_scene->scene->getMesh(triid);
    }
    if (solver_node.child("ContactPoints")) {
        contact_index = solver_node.child("ContactPoints").text().as_int();
        std::cout<<"contact index" << contact_index<<std::endl;
    }

}

void Lobo::GraspFingerSimulator::precompute() {
    DynamicSimulator::precompute();
}

void Lobo::GraspFingerSimulator::stepForward() {

    int num_tri_node = trimesh;

}

int Lobo::GraspFingerSimulator::getCurStep() {
    int tmp = 0;
    return tmp;
}